/*
 * globals.h
 *
 *  Created on: 24 oct. 2024
 *      Author: nicotina
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "u8g2_esp32_hal.h"
#include "u8g2.h"
#include "driver/gpio.h"
#include "botones.h"

// Función para navegación cíclica
int mod(int a, int n);
void mostrar_mensaje(const char *mensaje, bool aviso, menu_t menu_prev);

// Enum para los menús
typedef enum {
  MENU_PRINCIPAL, 
  MENU_USB, 
  MENU_NFC, 
  MENU_EMULAR, 
  MENU_DISPOSITIVO, 
  MENU_WIFI,
  MENU_BATERIA,
  MENU_RFID,
  MENU_LISTA_WIFI,
  MENU_ATAQUE_WIFI
} menu_t;

// Variables globales
extern int item_sel_idx;
extern bool refrescar;
extern menu_t menu_actual;
extern u8g2_t u8g2;  // Declaración externa de u8g2


#endif
