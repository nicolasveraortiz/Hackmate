/*
 * modulos.c
 *
 *  Created on: 24 oct. 2024
 *      Author: nicotina
 */
#include "globals.h"

// Implementación de la función mod
int mod(int a, int n) {
    int res = a % n;
    if (res < 0) res += n;
    return res;
}

// Funcion para mostrar mensajes en pantalla
void mostrar_mensaje(const char *mensaje, bool aviso) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawStr(&u8g2, 10, 20, mensaje);
    if (aviso){
        u8g2_DrawStr(&u8g2, 30, 60, "<- Volver");
        u8g2_DrawBox(&u8g2, 30, 49, 63, 11);  // Cuadro de "Volver"
        u8g2_SendBuffer(&u8g2);

        // Espera hasta que el boton "sel" sea presionado para "volver"
        while (gpio_get_level(PIN_BOT_SEL) == 1) {
            menu_actual = MENU_USB;
            item_sel_idx = 0; 
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
        // Agregar un pequeño retraso para evitar rebotes
    u8g2_SendBuffer(&u8g2);
    vTaskDelay(200 / portTICK_PERIOD_MS);
        
}

// Variables
int item_sel_idx = 0;
bool refrescar = true;
menu_t menu_actual = MENU_PRINCIPAL;

// Definir la estructura de la pantalla U8g2
u8g2_t u8g2;



