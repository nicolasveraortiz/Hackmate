/*
 * wifi.h
 *
 *  Created on: 11 nov. 2024
 *      Author: nicotina
 */

#ifndef MAIN_WIFI_H_
#define MAIN_WIFI_H_

#include "esp_wifi.h"
#include "wifi_controller.h"

static bool wifi_iniciado = false;
static int wifi_mode = -1;

int ieee80211_raw_frame_sanity_check(int32_t arg1, int32_t arg2, int32_t arg3);

int iniciar_wifi(const char *TAG, int mode);
bool esta_wifi();
void basta_wifi();

int iniciar_wifi_modo_sta();
int iniciar_wifi_modo_ap();


#endif /* MAIN_WIFI_H_ */
