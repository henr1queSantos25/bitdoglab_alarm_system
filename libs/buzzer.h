#include "pico/stdlib.h"

#define BUZZER1 10


void init_buzzer();
void buzzer_on(uint buzzer_pin, float frequency, uint duration_ms);
void ativarAlarme();
void atualizar_buzzer_alarme();
void buzzer_confirmacao();
