/*
 * wifi_bad_usb.h
 *
 *  Created on: 24 oct. 2024
 *      Author: nicotina
 */

#ifndef WIFI_BAD_USB_H
#define WIFI_BAD_USB_H

#include <stdbool.h>

// Prototipos de funciones
void conectar_bad_usb(int item_sel_idx);
void mostrar_mensaje(const char* mensaje, bool aviso);  // Nueva función para mostrar mensajes en pantalla

extern bool connected;
#endif


