#include "pico/stdlib.h"
#include "buttons.h"

void setup_buttons(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}

bool debounce_button(uint gpio) {
    if (!gpio_get(gpio)) { 
        sleep_ms(50); // Espera um tempo curto para estabilizar
        if (!gpio_get(gpio)) { 
            while (!gpio_get(gpio)); 
            return true; // Botão pressionado e solto corretamente
        }
    }
    return false; // Botão não foi pressionado
}