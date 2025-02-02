/*
 * wifi.c
 *
 *  Created on: 11 nov. 2024
 *      Author: nicotina
 */
#include "esp_log.h"
#include <string.h>
#include "nvs_flash.h"
#include "wifi_controller.h"
#include "esp_wifi.h"
#include "wifi.h"
#include "globals.h"

int ieee80211_raw_frame_sanity_check(int32_t arg1, int32_t arg2, int32_t arg3) { 
    return 0;
}

void basta_wifi() {
    if (!wifi_iniciado) return;
    wifi_iniciado = false;

    // Detener Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_stop());

    // Eliminar el controlador de eventos
    ESP_ERROR_CHECK(esp_event_loop_delete_default());

    // Destruir la interfaz de red
    esp_netif_t *netif = esp_netif_get_default_netif();
    if (netif != NULL) {
        esp_netif_destroy(netif);
    }

    // Desinicializar Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_deinit());
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
        mostrar_mensaje("Error Init NVS!", true, MENU_WIFI);
        goto error;
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Init Wi-Fi!", true, MENU_WIFI);
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
        mostrar_mensaje("Error Modo Wi-Fi!", true, MENU_WIFI);
        goto error;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error iniciando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Start Wi-Fi!", true, MENU_WIFI);
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
