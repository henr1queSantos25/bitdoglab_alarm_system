# Sistema de Alarme Embarcado com Modo de Monitoramento e Defesa

Este projeto tem como objetivo o desenvolvimento de um sistema de alarme embarcado utilizando a placa BitDogLab com o microcontrolador RP2040. O sistema opera em dois modos distintos — monitoramento e defesa — e visa demonstrar a integração de diversos periféricos de forma eficiente, segura e responsiva.

---

## Descrição Geral

O sistema permanece em modo de monitoramento enquanto o alarme estiver ativado. Neste estado, são utilizados sinais visuais (piscar do LED verde e efeito scanner na matriz de LEDs) para indicar que o sistema está ativo.

A transição para o modo de defesa ocorre mediante o acionamento de um botão dedicado, desde que o alarme esteja ligado. Nesse estado, o sistema emite sinais visuais e sonoros que simulam um alarme de invasão, utilizando LED RGB com efeito de pulsação, buzzer com alternância de frequências, e exibição de um símbolo de alerta ("X") piscante na matriz de LEDs.


---

## Funcionalidades Implementadas

- Ativação e desativação do alarme via botão físico.
- Detecção de invasão com validação de estado do alarme.
- Animações visuais específicas para cada modo de operação.
- Emissão de alarme sonoro com controle de tempo e frequência.
- Controle visual via matriz de LEDs 5x5.
- Exibição de quadrado 8x8 pixels no display OLED, controlado pelo joystick (funcionalidade obrigatória).

---

## Periféricos Utilizados

| Periférico               | Pino do RP2040     | Função no Sistema                                                                 |
|--------------------------|--------------------|------------------------------------------------------------------------------------|
| **Joystick (ADC)**       | GP26, GP27         | Controla a posição do quadrado no display OLED.                                   |
| **Display OLED (I2C)**   | SDA (GP14), SCL (GP15) | Exibe o quadrado de 8x8 pixels (função obrigatória).                         |
| **Botão Monitoramento / Cancelar Monitoramento** | GP5          | Ativa ou desativa o modo de monitoramento (alarme) e cancela o modo de defesa.    |
| **Botão Defesa / Cancelar Defesa**         | GP6            | Aciona o modo de defesa (somente se o alarme estiver ativado).                    |
| **Matriz de LEDs RGB 5x5** | GP7               | Exibe animações: scanner no monitoramento e "X" piscando na defesa.              |
| **LED RGB (vermelho e verde)** | GP13 (R), GP11 (G) | Verde piscando no monitoramento; vermelho com fade no modo de defesa.         |
| **Buzzer**               | GP10               | Emite alarme sonoro no modo de defesa com alternância de frequências.            |

---

## Requisitos Técnicos

- Microcontrolador: RP2040 (BitDogLab)
- Linguagem de Programação: C
- SDK: Pico SDK

---

## 📁 Estrutura do Projeto

- `bitdoglab_alarm_system.c` — Lógica principal do sistema (modos, entradas, animações)
- `libs/bitdoglab_alarm_system.pio` — Código PIO para controle eficiente da matriz de LEDs 5x5. 
- `libs/led_5x5.c/.h` — Controle da matriz de LEDs 5x5 via PIO
- `libs/buzzer.c/.h` — Geração de sons com buzzer
- `libs/ssd1306.c/.h` — Controle do display OLED via I2C
- `libs/buttons.c/.h` — Controle dos botões
- `libs/ledRGB.c/.h` — Controle do LED RGB via GPIO e PWM

---

## Demonstração

📽️ **Vídeo de demonstração:** [Link para o vídeo](https://drive.google.com/file/d/1M_Lhx5XzcIsgyD-bQIyQqbxiw13CxtsP/view)

---

## Execução

1. Suba o código na BitDogLab via VS Code com o ambiente do RP2040.
2. Pressione o botão 1 para ativar o alarme (modo monitoramento).
3. Pressione o botão 2 para simular uma invasão (modo de defesa).

---

## Conceitos Aplicados

- Controle de periféricos embarcados (ADC, I2C, GPIO, PIO, PWM)
- Interrupções com debounce por software
- Temporização não bloqueante (`absolute_time_t`)
- Efeitos visuais e sonoros sincronizados
- Separação de estados (monitoramento / defesa)

---

## Desenvolvido por

- **Henrique Oliveira dos Santos**  
- [LinkedIn](https://www.linkedin.com/in/dev-henriqueo-santos/)

---

## 📎 Links Úteis

- [Documentação do Pico SDK](https://raspberrypi.github.io/pico-sdk-doxygen/index.html)
- [Datasheet SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)


