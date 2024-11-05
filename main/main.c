#include "esp_wifi.h"
#include "grafica_menu.h"
#include "botones.h"
#include "wifi_bad_usb.h"
#include "globals.h"
#include "driver/gpio.h"


void app_main() {
    // Inicialización de la pantalla U8g2
    init_display(&u8g2);
    configurar_botones();
    // Inicialización de Wi-Fi (en modo apagado por defecto)
    esp_wifi_stop();

    // Ciclo principal
    while (true) {
        // Leer los botones y actualizar el estado del menú
        leer_botones();

        // Renderizar el menú en la pantalla si se requiere actualización
        if (refrescar) {
            render_menu(&u8g2, menu_actual);  // Llamada para renderizar el menú
            refrescar = false;
            vTaskDelay(300 / portTICK_PERIOD_MS);  // Pausa para evitar rebotes
        }
    }
}

