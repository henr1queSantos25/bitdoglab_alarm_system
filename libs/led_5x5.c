#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "led_5x5.h"

// ARQUIVO .pio
#include "build/bitdoglab_alarm_system.pio.h"

// NÚMERO DE LEDS
#define NUM_PIXELS 25

// PINO DA MATRIZ DE LED
#define OUT_PIN 7

PIO pio;
uint sm; 


// INICIALIZAÇÃO E CONFIGURAÇÃO DO PIO
void setup_PIO() {
    pio = pio0;
    uint offset = pio_add_program(pio, &bitdoglab_alarm_system_program);
    sm = pio_claim_unused_sm(pio, true);
    bitdoglab_alarm_system_program_init(pio, sm, offset, OUT_PIN);
    matrix_rgb(0.0, 0.0, 0.0); // Inicializa a matriz com todos os LEDs apagados
}

// FUNÇÃO PARA ENVIAR O VALOR RGB PARA A MATRIZ
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// ACENDE TODOS OS LEDS COM UMA COR ESPECÍFICA
void drawMatrix(uint cor) {
    uint32_t valor_led;

    /*
    "0. Verde", 
    "1. Vermelho", 
    "2. Azul", 
    "3. Amarelo", 
    "4. Roxo,    
    "5. Desligado"
    */

    switch (cor)
    {
    case 0:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.2);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 1:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.2, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 2:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.2, 0.0, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 3:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.6, 0.2);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 4:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.2, 0.2, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    case 5:
        for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
            pio_sm_put_blocking(pio0, 0, valor_led);
        }
        break;
    }
    
    

}

// SCANNER HORIZONTAL
int linha_atual = 0;
bool descendo = true;
absolute_time_t ultimo_update_scanner;

void atualizarScannerHorizontal() {
    const uint32_t cor = matrix_rgb(0.0, 0.0, 0.01); // Verde médio
    const uint32_t apagado = matrix_rgb(0.0, 0.0, 0.0);

    // Só atualiza a cada 150ms
    if (!time_reached(ultimo_update_scanner)) return;
    ultimo_update_scanner = make_timeout_time_ms(150);

    for (int i = 0; i < NUM_PIXELS; i++) {
        if (i / 5 == linha_atual)
            pio_sm_put_blocking(pio0, 0, cor);
        else
            pio_sm_put_blocking(pio0, 0, apagado);
    }

    if (descendo) {
        linha_atual++;
        if (linha_atual >= 5) {
            linha_atual = 3; // volta a subir
            descendo = false;
        }
    } else {
        linha_atual--;
        if (linha_atual < 0) {
            linha_atual = 1; // volta a descer
            descendo = true;
        }
    }

}

// ANIMAÇÃO DO ALARME (X)
absolute_time_t proximo_flash_x;
bool x_aceso = false;

void animacao_alarme_x() {
    if (!time_reached(proximo_flash_x)) return;

    proximo_flash_x = make_timeout_time_ms(200); // Pisca a cada 200 ms
    x_aceso = !x_aceso;

    uint32_t cor = x_aceso ? matrix_rgb(0.0, 0.05, 0.0) : matrix_rgb(0.0, 0.0, 0.0); // vermelho / apagado

    for (int i = 0; i < NUM_PIXELS; i++) {
        // Índices que formam o X
        if (i == 0 || i == 4 || i == 6 || i == 8 || i == 12 || i == 16 || i == 18 || i == 20 || i == 24)
            pio_sm_put_blocking(pio0, 0, cor);
        else
            pio_sm_put_blocking(pio0, 0, matrix_rgb(0.0, 0.0, 0.0)); // Apaga os demais
    }
}