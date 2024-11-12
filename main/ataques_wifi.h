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

void get_aps();
const char *rssi_to_icon(int8_t rssi);
wifi_ap_record_t *ap_selected;

// Funciones
void attack_dos_stop();
void attack_dos_start(wifi_ap_record_t *ap_record);
void send_deauth_frame(const wifi_ap_record_t *ap_record);

#endif /* MAIN_ATAQUES_WIFI_H_ */
