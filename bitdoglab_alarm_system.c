#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// Bibliotecas externas
#include "libs/ssd1306.h"
#include "libs/buttons.h"
#include "libs/led_5x5.h"
#include "libs/buzzer.h"

// Configuraçãp do display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define SSD1306_ADDRESS 0x3C
ssd1306_t ssd;
bool cor_display = false;
#define tamanho_quadrado 8 // Tamanho do quadrado em pixels

// Configuração do joystick
#define VRY 26
#define VRX 27
#define ADC_CHANNEL_0 0
#define ADC_CHANNEL_1 1

// Configuração dos botões e dos LEDs
#define BUTTON_ligarAlarme 5
#define BUTTON_modoDefesa 6
#define LED_PIN_GREEN 11
#define LED_PIN_RED 13

// Variáveis usadas na interrupção
uint volatile last_time = 0;
bool volatile alarme_ativo = false; // Variável para armazenar o estado do alarme
bool volatile modo_defesa = false;  // Variável para armazenar o estado do modo de defesa

// Protótipos das funções
void setup_joystick();
void setup();
void posicao_joystick(uint16_t *value_x, uint16_t *value_y);
void mover_quadrado(ssd1306_t *ssd, uint16_t value_x, uint16_t value_y);
void draw_square(int16_t x, int16_t y);
void gpio_irq_handler(uint gpio, uint32_t events);
void setupLED(uint led);
void piscar_led_verde();
void setup_pwm_led(uint led);
void atualizar_fade_led_red();

int main() {
    uint16_t value_x, value_y;
    setup();

    // Interrupção
    gpio_set_irq_enabled_with_callback(BUTTON_ligarAlarme, GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_modoDefesa, GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);

    while (true) {

        draw_square(value_x, value_y); // Chama a função para desenhar o quadrado na nova posição

        if (modo_defesa) {
            atualizar_buzzer_alarme(); // Chama a função para tocar o alarme
            animacao_alarme_x(); // Chama a função para piscar o X na matriz de LED
            gpio_put(LED_PIN_GREEN, 0);
            atualizar_fade_led_red();
        } else if (alarme_ativo) {
            pwm_set_gpio_level(LED_PIN_RED, 0); // Desliga o LED vermelho
            atualizarScannerHorizontal(); // Atualiza o scanner horizontal se o alarme estiver ativo
            piscar_led_verde(); // Pisca o LED verde se o alarme estiver ativo
        } else {
            drawMatrix(5); // Desenha a matriz de LED com a cor padrão (desligado)
            gpio_put(LED_PIN_GREEN, 0); // Desliga o LED verde
        }

        sleep_us(10); // Delay para evitar leituras muito rápidas
    }
}

/* ==========================================================
   FUNÇÕES DE SETUP
   ========================================================== */
void setup() {
    stdio_init_all();
    setup_joystick();
    setup_I2C(I2C_PORT, I2C_SDA, I2C_SCL);
    setup_ssd1306(&ssd, SSD1306_ADDRESS, I2C_PORT); // Inicializa o display
    setup_buttons(BUTTON_ligarAlarme);              // Configura o botão de ligar o alarme
    setup_buttons(BUTTON_modoDefesa);               // Configura o botão de modo de defesa
    setup_PIO();                                    // Configura o PIO para a matriz de LED
    setupLED(LED_PIN_GREEN);                        // Configura o LED verde
    setup_pwm_led(LED_PIN_RED);                     // Configura o LED vermelho
    init_buzzer();                                 // Inicializa o buzzer
}

void setupLED(uint led) {
    gpio_init(led);              // Inicializa o pino do LED
    gpio_set_dir(led, GPIO_OUT); // Define o pino como saída
    gpio_put(led, 0);            // Desliga o LED inicialmente
}

void setup_pwm_led(uint led) {
    gpio_set_function(led, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(led);
    pwm_set_wrap(slice_num, 255);
    pwm_set_enabled(slice_num, true);
}

void setup_joystick() {
    adc_init();
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);
}

/* ==========================================================
   FUNÇÕES QUE CONTROLAM O QUADRADO 8x8 NO DISPLAY
   ========================================================== */
void posicao_joystick(uint16_t *value_x, uint16_t *value_y) {

    // Leitura do valor do eixo X do joystick
    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *value_x = adc_read();           // Lê o valor do eixo X (0-4095)

    // Leitura do valor do eixo Y do joystick
    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *value_y = adc_read();           // Lê o valor do eixo Y (0-4095)
}

void mover_quadrado(ssd1306_t *ssd, uint16_t value_x, uint16_t value_y) {

    // Mapear os valores do joystick para as coordenadas do display
    int posicao_x = (value_x * (WIDTH - tamanho_quadrado)) / 4096;
    int posicao_y = ((4096 - value_y) * (HEIGHT - tamanho_quadrado)) / 4096; // Inverte o eixo Y

    // Desenhar o quadrado na nova posição
    draw_filled_square(ssd, posicao_x, posicao_y);
}

void draw_square(int16_t x, int16_t y) {
    posicao_joystick(&x, &y);         // Chama a função para ler os valores do joystick
    ssd1306_fill(&ssd, !cor_display); // Limpa o display antes de desenhar o quadrado

    ssd1306_rect(&ssd, 1, 1, 126, 62, cor_display, !cor_display); // Desenha o retângulo de borda
    mover_quadrado(&ssd, x, y);                                   // Chama a função para mover o quadrado

    ssd1306_send_data(&ssd); // Envia os dados para o display
}

/* ==========================================================
   INTERRUPÇÃO DOS BOTÕES
   ========================================================== */
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t time = to_us_since_boot(get_absolute_time());

    if (time - last_time > 500000) {

        if (gpio == BUTTON_ligarAlarme && !modo_defesa) {
            alarme_ativo = !alarme_ativo;
            printf("Alarme %s\n", alarme_ativo ? "Ativado!" : "Desativado!");
        }

        if (gpio == BUTTON_modoDefesa && alarme_ativo) {
            modo_defesa = !modo_defesa;
            ativarAlarme();
            printf("%s\n", modo_defesa ? "Invasão detectada! Iniciando contra-medidas..." : "Modo de Defesa Desativado!");
        }

        last_time = time; // atualiza o tempo
    }
}

/* ==========================================================
   FUNÇÃO PARA PISCAR O LED
   ========================================================== */

//FUNÇÃO PARA PISCAR O LED VERDE
absolute_time_t proximo_piscar; // Variável para armazenar o tempo do próximo piscar do LED
bool led_verde_aceso = false; // Variável para armazenar o estado do LED verde

void piscar_led_verde() {
    if (!time_reached(proximo_piscar))
        return;

    // Inverte o estado atual
    led_verde_aceso = !led_verde_aceso;
    gpio_put(LED_PIN_GREEN, led_verde_aceso);

    // Define próximo momento de alternância (ex: a cada 500 ms)
    proximo_piscar = make_timeout_time_ms(500);
}

// FUNÇÃO PARA FAZER O LED VERMELHO FADE
int brilho_red = 0; // Brilho do LED vermelho (0-255)
bool subindo = true; // Variável para controlar a direção do fade
absolute_time_t proximo_fade; // Variável para armazenar o tempo do próximo fade

void atualizar_fade_led_red() {
    if (!time_reached(proximo_fade)) return;

    pwm_set_gpio_level(LED_PIN_RED, brilho_red);

    // Atualiza valor do brilho
    if (subindo) {
        brilho_red += 5;
        if (brilho_red >= 255) {
            brilho_red = 255;
            subindo = false;
        }
    } else {
        brilho_red -= 5;
        if (brilho_red <= 0) {
            brilho_red = 0;
            subindo = true;
        }
    }

    // Define próximo momento do fade
    proximo_fade = make_timeout_time_ms(15); // controla a suavidade da transição
}