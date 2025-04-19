#include "buzzer.h"
#include "pico/stdlib.h"


// VARIÁVEIS GLOBAIS
bool buzzer_tom_alto = false;
absolute_time_t proximo_bipe;
bool alarme_sonando = false;

void init_buzzer() {
    gpio_init(BUZZER1);
    gpio_set_dir(BUZZER1, GPIO_OUT);
}

// Função para gerar som no buzzer por um tempo específico e com a frequência desejada
void buzzer_on(uint buzzer_pin, float frequency, uint duration_ms) {
    float period = 1.0f / frequency;   // Calcula o período da onda
    float half_period = period / 2;    // Meio período em segundos
    uint cycles = (duration_ms * frequency) / 1000; // Número de ciclos necessários

    for (uint i = 0; i < cycles; i++)
    {
        gpio_put(buzzer_pin, 1);            // Liga o buzzer (HIGH)
        sleep_us((int)(half_period * 1e6)); // Aguarda meio período
        gpio_put(buzzer_pin, 0);            // Desliga o buzzer (LOW)
        sleep_us((int)(half_period * 1e6)); // Aguarda meio período
    }
}


void ativarAlarme(){
    alarme_sonando = true;
    proximo_bipe = make_timeout_time_ms(0); // dispara imediatamente
}

void atualizar_buzzer_alarme() {
    if (!alarme_sonando) return;

    if (!time_reached(proximo_bipe)) return;

    float freq = buzzer_tom_alto ? 1000.0f : 500.0f;
    buzzer_tom_alto = !buzzer_tom_alto;

    // Gera um bipe de 50ms com a frequência escolhida
    buzzer_on(BUZZER1, freq, 50);

    // Define o próximo bipe daqui a 200ms
    proximo_bipe = make_timeout_time_ms(100);

}


// Função para gerar um som curto de confirmação
void buzzer_confirmacao() {
    float freq_inicial = 800.0f;  // Frequência inicial
    float freq_final = 1200.0f;   // Frequência final
    uint duracao_bip = 80;        // Duração de cada bip em ms
    uint intervalo = 50;          // Pequena pausa entre bipes

    // Três bipes curtos e crescentes
    for (int i = 0; i < 3; i++) {
        buzzer_on(BUZZER1, freq_inicial + (i * 100), duracao_bip);
        sleep_ms(intervalo);
    }
}
