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

// Funciones Gubai-Fai
int iniciar_guaifai(const char *TAG);
void basta_guaifai();
bool esta_guaifai();

// Funciones
void attack_dos_stop();
void attack_dos_start(wifi_ap_record_t ap_record);
static void timer_send_deauth_frame(void *arg);
void attack_method_broadcast(const wifi_ap_record_t *ap_record, unsigned period_sec);
void attack_method_broadcast_stop();
void attack_method_rogueap(const wifi_ap_record_t *ap_record);

#endif /* MAIN_ATAQUES_WIFI_H_ */