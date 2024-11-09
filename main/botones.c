#include "botones.h"
#include "ataques_wifi.h"
#include "grafica_menu.h"
#include "wifi_bad_usb.h"
#include "driver/gpio.h"
#include "globals.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <stdio.h>


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
        // Aca tendria que apagar el WiFi 
          break;
        }

        if (item_sel_idx <= 3) {
          conectar_bad_usb(item_sel_idx);
        }
        break;


      case MENU_NFC:
        if (item_sel_idx == 0) {
          printf("clonar_dialog()");
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
          printf("Opciones de gemelo malvado");
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
            basta_guaifai();
        }
        else if (item_sel_idx > 0) {
            menu_actual = MENU_ATAQUE_WIFI;
            ap_selected = ap_list->records[item_sel_idx-1];
            item_sel_idx = 0;
        }
        break;
      
      case MENU_ATAQUE_WIFI:
        if (item_sel_idx == 0) {
              printf("Definir tiempo");
          }
        else if (item_sel_idx == 1) {
            for (int i=0; i<6; ++i) {
                printf("%.2x%c", ap_selected.bssid[i], (i == 5) ? '\n' : ':');
            }
            attack_dos_start(ap_selected);
        }
        else if (item_sel_idx == MENU_ATAQUE_NUM_ITEMS - 1){
              menu_actual = MENU_LISTA_WIFI;
              item_sel_idx = 0;
        }
        break;
        
      case MENU_DISPOSITIVO:
        if (item_sel_idx == 0) {
          printf("Emulando dispositivo");
        } else if (item_sel_idx == 1) {
          printf("Eliminando dispositivo");
        } else if (item_sel_idx == 2) {
          menu_actual = MENU_EMULAR;
          item_sel_idx = 0;
        }
        break;

      case MENU_BATERIA:
        if (item_sel_idx == 0) {
          printf("Ver nivel");
        } else if (item_sel_idx == 1) {
          printf("Modo ahorro");
        } else if (item_sel_idx == 2) {
          menu_actual = MENU_PRINCIPAL;
          item_sel_idx = 0;
        }
        break; 

      case MENU_RFID:
        if (item_sel_idx == 0) {
          printf("Clonar senal");
        } else if (item_sel_idx == 1) {
          printf("Emular senal");
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
// Pregunta: las variables como referencia o item_sel_idx, que se usan en multiples lugares, deberian ir a modulos?
