#include "grafica_menu.h"
#include "botones.h"
#include "globals.h"

void app_main() {
    // Inicialización de la pantalla U8g2
    init_display(&u8g2);
    configurar_botones();
	mostrar_splash(&u8g2);
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

