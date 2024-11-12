#ifndef BEACON_SPAM_H
#define BEACON_SPAM_H
/*
 * beacon_spam.h
 *
 *  Created on: 11 nov. 2024
 *      Author: nicotina
 */

void generar_ssid(uint8_t *out);
void generar_mac(uint8_t *out);
void send_random_beacon();
void attack_beacon_spam();

#endif
