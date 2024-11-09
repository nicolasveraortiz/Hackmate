/*
 * ataques_wifi.h
 *
 *  Created on: 28 oct. 2024
 *      Author: nicotina
 */

#ifndef MAIN_ATAQUES_WIFI_H_
#define MAIN_ATAQUES_WIFI_H_

#include "esp_wifi_types.h"
#include "wifi_controller.h"

const wifictl_ap_records_t *get_aps();
const char *rssi_to_icon(int8_t rssi);
wifi_ap_record_t ap_selected;

// Funciones WiFi
int iniciar_wifi(const char *TAG, int mode);
void basta_wifi();
bool esta_wifi();

// Funciones
void attack_dos_stop();
void attack_dos_start(wifi_ap_record_t ap_record);
void send_deauth_frame(void *ap_record);

#endif /* MAIN_ATAQUES_WIFI_H_ */
