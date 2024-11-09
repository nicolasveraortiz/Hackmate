/*
 * menu_graficos.c
 *
 *  Created on: 24 oct. 2024
 *      Author: nicotina
 */

#include "grafica_menu.h"
#include "globals.h"
#include "grafica_menu.h"
#include "u8g2_esp32_hal.h"
#include "u8g2.h"
#include "wifi_controller.h"
#include "ataques_wifi.h"

void init_display(u8g2_t *u8g2) {
    // Configuración de pines I2C para ESP32
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.i2c.sda = 21;  // Pin SDA
    u8g2_esp32_hal.bus.i2c.scl = 22;  // Pin SCL
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    // Inicializar u8g2 con SH1106 y la dirección 0x3C (0x78 en 8 bits)
    u8g2_Setup_sh1106_i2c_128x64_noname_f(
        u8g2, U8G2_R0, u8g2_esp32_i2c_byte_cb, u8g2_esp32_gpio_and_delay_cb);

    // Establecer la dirección (0x3C o 0x3D según la configuración)
    u8g2_SetI2CAddress(u8g2, 0x3C << 1);  // Usa 0x3D si prefieres la otra dirección

    // Inicializar la pantalla
    u8g2_InitDisplay(u8g2);
    u8g2_ClearBuffer(u8g2);
    u8g2_SetPowerSave(u8g2, 0);  // Activar la pantalla
    u8g2_SetDrawColor(u8g2, 2);
}

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 240)
char menu_items[][20] =
  { "Bad USB", "Bateria", "NFC", "RFID", "Wi-Fi" };
char usb_menu_items[][20] =
  { "Forzar Apagado", "Matar Interfaz", "Agotar recursos", "Rick Roll", "<- Volver" };
char nfc_menu_items[][20] =
  { "Clonar disp.", "Emular disp.", "<- Volver" };
char nfc_submenu_emular_items[][20] =
  { "Dispositivo1", "Dispositivo2", "Dispositivo3", "<- Volver" };
char nfc_submenu_device_items[][20] =
  { "Emular", "Eliminar", "<- Volver" };
char wifi_menu_items[][20] =
  { "Desautenticar", "Gemelo Malvado", "<- Volver" };
char battery_menu_items[][20] = 
  { "Ver nivel", "Apagar", "<- Volver" };
char rfid_menu_items[][20] =
  { "Clonar senal", "Emular senal", "<- Volver" };
char wifi_ataque_menu_items[][20] =
  { "Tiempo", "Iniciar Ataque", "<- Volver" };

wifictl_ap_records_t *ap_list;

const int MAIN_NUM_ITEMS = 5;
const int USB_NUM_ITEMS = 5;
const int NFC_NUM_ITEMS = 3;
const int NFC_SUBMENU_EMULAR_NUM_ITEMS = 4;
const int NFC_SUBMENU_DEVICE_NUM_ITEMS = 3;
const int WIFI_MENU_NUM_ITEMS = 3;
const int BATTERY_MENU_NUM_ITEMS = 3;
const int RFID_MENU_NUM_ITEMS = 3;
const int MENU_ATAQUE_NUM_ITEMS = 3;

void update_ap_list() {
    mostrar_mensaje ("Buscar redes...", false);
    ap_list = get_aps();
}

// Función para obtener el número de elementos del menú
int get_num_items_for_menu(menu_t menu) {
  switch (menu) {
    case MENU_PRINCIPAL:
      return MAIN_NUM_ITEMS;  // Número de elementos en el menú principal

    case MENU_USB:
      return USB_NUM_ITEMS;  // Número de elementos en el menú USB
    
    case MENU_BATERIA:
      return BATTERY_MENU_NUM_ITEMS;
    
    case MENU_RFID:
      return RFID_MENU_NUM_ITEMS;

    case MENU_NFC:
      return NFC_NUM_ITEMS;  // Número de elementos en el menú NFC

    case MENU_EMULAR:
      return NFC_SUBMENU_EMULAR_NUM_ITEMS;  // Número de elementos en el submenú de emulación NFC

    case MENU_DISPOSITIVO:
      return NFC_SUBMENU_DEVICE_NUM_ITEMS;  // Número de elementos en el menú del dispositivo NFC

    case MENU_WIFI:
      return WIFI_MENU_NUM_ITEMS;

    case MENU_LISTA_WIFI:
      return ap_list->count + 1;
      
    case MENU_ATAQUE_WIFI:
        return MENU_ATAQUE_NUM_ITEMS;
        
    default:
      return MAIN_NUM_ITEMS;  
  }
}

const char *get_items_for_menu(menu_t menu, int idx) {
  switch (menu) {
    case MENU_PRINCIPAL:
      return menu_items[idx];

    case MENU_USB:
      return usb_menu_items[idx];

    case MENU_BATERIA:
      return battery_menu_items[idx];

    case MENU_RFID:
      return rfid_menu_items[idx];

    case MENU_NFC:
      return nfc_menu_items[idx];

    case MENU_EMULAR:
      return nfc_submenu_emular_items[idx];

    case MENU_DISPOSITIVO:
      return nfc_submenu_device_items[idx];

    case MENU_WIFI:
      return wifi_menu_items[idx];

    case MENU_LISTA_WIFI: {
      if (idx == 0) return "<- Volver";
      return (const char *)(ap_list->records[idx-1].ssid);
    }
    
    case MENU_ATAQUE_WIFI:
        return wifi_ataque_menu_items[idx];
    
    default:
      return menu_items[idx];
  }
}
// Función para obtener el ícono asociado al menú
const uint8_t *get_icon_bitmap(menu_t menu, int idx) {
  switch (menu) {
    case MENU_PRINCIPAL:
      return bitmap_icons[idx];
    case MENU_LISTA_WIFI:
      if (idx == 0) return NULL;
      return (const uint8_t *)rssi_to_icon(ap_list->records[idx-1].rssi);
    default:
      return NULL;
  }
  return NULL;
}

// Función para renderizar el menú en la pantalla
void render_menu(u8g2_t *u8g2, menu_t menu) {
    int num_items = get_num_items_for_menu(menu);
    item_sel_idx = mod(item_sel_idx, num_items);
    
    u8g2_ClearBuffer(u8g2); // Limpiar pantalla

    for (int i = 0; i < 3; ++i) {
        int item_idx = mod(item_sel_idx + i - 1, num_items);
        const char* menu_item_str = get_items_for_menu(menu, item_idx);
        const uint8_t* icon = get_icon_bitmap(menu, item_idx);

        // Si el ítem está seleccionado
        if (i == 1) {
            u8g2_SetFont(u8g2, u8g2_font_7x14B_tf);  // Seleccionado en negrita
        } else {
            u8g2_SetFont(u8g2, u8g2_font_7x14_tf);
        }

        if (icon != NULL) {
            u8g2_DrawBitmap(u8g2, 4, 2 + 22*i, 16/8, 16, icon);  // Dibuja el ícono
            u8g2_DrawStr(u8g2, 26, 15 + 22*i, menu_item_str);  // Dibuja el texto
        } else {
            u8g2_DrawStr(u8g2, 5, 15 + 22*i, menu_item_str);  // Dibuja el texto sin ícono
        }
    }

    // Dibuja el marco del ítem seleccionado
    u8g2_DrawFrame(u8g2, 0, 21, 124, 22);
    u8g2_DrawFrame(u8g2, 1, 22, 122, 20);

    u8g2_SendBuffer(u8g2);  // Enviar a la pantalla
}

// Función para limpiar pantalla al cambiar entre menús
void clear_and_render_menu() {  
  u8g2_ClearBuffer(&u8g2);  // Limpia el buffer de la pantalla antes de redibujar
  render_menu(&u8g2, menu_actual);
  u8g2_SendBuffer(&u8g2);  // Envía el contenido del buffer a la pantalla
}



