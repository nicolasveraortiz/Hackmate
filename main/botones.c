#include "botones.h"
#include "ataques_wifi.h"
#include "esp_sleep.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "u8g2.h"
#include "wifi.h"
#include "beacon_spam.h"
#include "grafica_menu.h"
#include "wifi_bad_usb.h"
#include "driver/gpio.h"
#include "globals.h"


// Función para configurar los pines de los botones
void configurar_botones() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_BOT_SEL) | (1ULL << PIN_BOT_UP) | (1ULL << PIN_BOT_DOWN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
}

// Función para leer los botones y actualizar el estado del menú
void leer_botones() {
    bool up = !gpio_get_level(PIN_BOT_UP);
    bool down = !gpio_get_level(PIN_BOT_DOWN);
    bool sel = !gpio_get_level(PIN_BOT_SEL);

  if (up) {
    --item_sel_idx;
  } else if (down) {
    ++item_sel_idx;
  }

  if (sel) {
    switch (menu_actual) {
      case MENU_PRINCIPAL:
        if (item_sel_idx == 0) {
          menu_actual = MENU_USB;
          item_sel_idx = 0;
        }
        else if (item_sel_idx == 1) {
          menu_actual = MENU_BATERIA;
          item_sel_idx = 0;
        }
        else if (item_sel_idx == 2) {
          menu_actual = MENU_NFC;
          item_sel_idx = 0;
        }
        else if (item_sel_idx == 3) {
          menu_actual = MENU_RFID;
          item_sel_idx = 0;
        }
        else if (item_sel_idx == 4) { 
          menu_actual = MENU_WIFI;
          item_sel_idx = 0;
          }
        break;

      case MENU_USB:
        // Opción para volver al menú principal
        if (item_sel_idx == USB_NUM_ITEMS - 1) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 0;
          break;
        }

        if (item_sel_idx <= 3) {
          conectar_bad_usb(item_sel_idx);
        }
        break;


      case MENU_NFC:
        if (item_sel_idx == 0) {
          mostrar_mensaje("En desarrollo!", true, MENU_NFC);
        } else if (item_sel_idx == 1) {
          menu_actual = MENU_EMULAR;
          item_sel_idx = 0;
        } else if (item_sel_idx == 2) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 2;
        }
        break;

      case MENU_EMULAR:
        if (item_sel_idx < NFC_SUBMENU_EMULAR_NUM_ITEMS - 1) {
          menu_actual = MENU_DISPOSITIVO;
          item_sel_idx = 0;
        } else if (item_sel_idx == NFC_SUBMENU_EMULAR_NUM_ITEMS - 1) {
          menu_actual = MENU_NFC;
          item_sel_idx = 1;
        }
        break;

      case MENU_WIFI:
        if(item_sel_idx == 0) {
          update_ap_list();
          menu_actual = MENU_LISTA_WIFI;
          item_sel_idx = 0;
        }
        else if (item_sel_idx == 1) {
			mostrar_mensaje("Beacon Spam act...", false, menu_actual);
            attack_beacon_spam();
        }
        else if (item_sel_idx == 2) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 0;
        }
        break;

      case MENU_LISTA_WIFI:
        if (item_sel_idx == 0) {
            menu_actual = MENU_WIFI;
            item_sel_idx = 0;
            basta_wifi();
        }
        else if (item_sel_idx > 0) {
            ap_selected = wifictl_get_ap_record(item_sel_idx-1);
            attack_dos_start(ap_selected);
            item_sel_idx = 0;
        }
        break;
        
      case MENU_DISPOSITIVO:
        if (item_sel_idx == 0) {
          mostrar_mensaje("En desarrollo!", true, MENU_DISPOSITIVO);
        } else if (item_sel_idx == 1) {
          mostrar_mensaje("En desarrollo!", true, MENU_DISPOSITIVO);
        } else if (item_sel_idx == 2) {
          menu_actual = MENU_EMULAR;
          item_sel_idx = 0;
        }
        break;

      case MENU_BATERIA:
        if (item_sel_idx == 0) {
          mostrar_mensaje("En desarrollo!", true, MENU_BATERIA);
        } 
        else if (item_sel_idx == 1) {
		  esp_sleep_enable_ext0_wakeup(PIN_BOT_SEL, 0);
		  mostrar_mensaje("Apagando...", false, menu_actual);
		  vTaskDelay(1500 / portTICK_PERIOD_MS);
		  u8g2_ClearBuffer(&u8g2);
		  u8g2_SendBuffer(&u8g2);
		  vTaskDelay(1000 / portTICK_PERIOD_MS);
          esp_deep_sleep_start();
        } 
        else if (item_sel_idx == 2) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 0;
        }
        break; 

      case MENU_RFID:
        if (item_sel_idx == 0) {
          mostrar_mensaje("En desarrollo!", true, MENU_RFID);
        } else if (item_sel_idx == 1) {
          mostrar_mensaje("En desarrollo!", true, MENU_RFID);
        } else if (item_sel_idx == 2) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 0;
        }
        break; 
    }
  }

  item_sel_idx = mod(item_sel_idx, get_num_items_for_menu(menu_actual));
  
  if (up || down || sel) {
    refrescar = true;
  }

    // Limitar el índice seleccionado usando la función mod
    item_sel_idx = mod(item_sel_idx, get_num_items_for_menu(menu_actual));

    // Solicitar refresco de pantalla si se presionó un botón
    if (up || down || sel) {
        refrescar = true;
    }
}
