#ifndef GRAFICA_MENU_H
#define GRAFICA_MENU_H

#include <stdbool.h>
#include <stdint.h>
#include "globals.h"
#include "icons.h"
#include "wifi_controller.h"

// Constantes de los menús
extern const int MAIN_NUM_ITEMS;
extern const int USB_NUM_ITEMS;
extern const int NFC_NUM_ITEMS;
extern const int NFC_SUBMENU_EMULAR_NUM_ITEMS;
extern const int NFC_SUBMENU_DEVICE_NUM_ITEMS;
extern const int WIFI_MENU_NUM_ITEMS;
extern const int PAYLOAD_NUM;
extern const int BATTERY_MENU_NUM_ITEMS;
extern const int RFID_MENU_NUM_ITEMS;
extern const int MENU_ATAQUE_NUM_ITEMS;

// Declaraciones de ítems del menú
extern char menu_items[][20];
extern char usb_menu_items[][20];
extern char nfc_menu_items[][20];
extern char nfc_submenu_emular_items[][20];
extern char nfc_submenu_device_items[][20];
extern char wifi_menu_items[][20];
extern char rfid_menu_items[][20];
extern char battery_menu_items[][20];
extern char wifi_ataque_menu_items[][20];

wifictl_ap_records_t *ap_list;

// Prototipos de funciones
void update_ap_list();
void init_display(u8g2_t *u8g2);
void render_menu(u8g2_t *u8g2, menu_t menu);
const uint8_t* get_icon_bitmap(menu_t menu, int idx);
int get_num_items_for_menu(menu_t menu);
const char* get_items_for_menu(menu_t menu, int idx);
void clear_and_render_menu();

#endif /* MAIN_GRAFICA_MENU_H_ */
