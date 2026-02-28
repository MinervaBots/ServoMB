# MinervaBots Servo ESP32 Library 🤖⚙️

<div align="center">
  <a href="https://registry.platformio.org/libraries/minervabots/MinervaBots-Servo-ESP32-Library"><img src="https://badges.registry.platformio.org/packages/minervabots/library/MinervaBots-Servo-ESP32-Library.svg" alt="PlatformIO Registry" /></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT" /></a>
</div>

## 📖 Sobre

Biblioteca desenvolvida pela equipe de competição de robótica **MinervaBots da Universidade Federal do Rio de Janeiro (UFRJ)** para o controle de precisão de Servo motores utilizando microcontroladores da família ESP32. 

Ao invés de depender de rotinas de software ou bibliotecas genéricas do Arduino, esta biblioteca acessa diretamente a API nativa `ledc` (LED Control) do **ESP-IDF**, garantindo sinais PWM de hardware extremamente estáveis, livres de *jitter* e sem bloquear o processamento do sistema operacional (FreeRTOS).

---

## ✨ Principais Características

* **PWM via Hardware Puro:** Controle de posição fluido usando a API nativa da Espressif.
* **Lazy Initialization (Segurança para RTOS):** A inicialização do hardware ocorre dinamicamente no momento exato do uso, prevenindo *Kernel Panics* causados pela chamada de construtores globais antes da subida do FreeRTOS.
* **Proteção contra Falhas:** Validação de pinos e canais para evitar lixo de memória ou travamentos do microcontrolador.
* **Multi-Compatibilidade:** Suporte arquitetural automático para o ESP32 clássico e para os novos chips sem hardware *High Speed* (ESP32-S2, S3, C3).
* **Frameworks Suportados:** Funciona perfeitamente tanto em projetos baseados em **ESP-IDF** puro quanto em **Arduino Core**.

---

## 📦 Instalação

A biblioteca está disponível oficialmente no [Registry do PlatformIO](https://registry.platformio.org/). Para instalar, basta adicionar o nome da biblioteca ao seu arquivo `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
framework = espidf  ; ou arduino
monitor_speed = 115200
lib_deps =
    minervabots/MinervaBots-Servo-ESP32-Library @ ^2.0.0
```

---

## 📖 Referência da API

A documentação oficial e completa da biblioteca se encontra no site [MinervaCore](https://minervabots.github.io/MinervaCore/docs/programacao/libraries/servoesp32)!

#### `ServoMB(uint8_t pino, uint8_t canal)`

Construtor da classe.

* **`pino`:** O pino GPIO físico onde o sinal do servo está conectado.
* **`canal`:** O canal PWM do LEDC a ser utilizado (0 a 15 no ESP32 clássico, 0 a 7 nos S2/S3/C3).

#### `void setPosicao(uint8_t posicao)`

Move o servo para o ângulo desejado.

* **`posicao`:** Valor em graus entre `0` e `180`. Valores fora do escopo são automaticamente limitados por segurança. O hardware só é ativado na primeira vez que esta função é chamada.

#### `uint8_t getPosicao() const`

Retorna a última posição (em graus) enviada ao motor.

---

## 🚀 Como Usar (Quick Start)

A utilização foi desenhada para ser a mais limpa possível. Veja o exemplo clássico de varredura (**Sweep**), movimentando o motor de 0° a 180° graus.

### Com Arduino

```cpp
#include <Arduino.h>
#include <ServoMB.h>

// Instancia o servo no pino 18, utilizando o canal PWM 0
ServoMB meuServo(18, 0);

void setup() {}

void loop() {
    // Varredura de 0 a 180 graus
    for (int pos = 0; pos <= 180; pos++) {
        meuServo.setPosicao(pos);
        delay(15); // Aguarda 15ms
    }

    // Varredura de 180 a 0 graus
    for (int pos = 180; pos >= 0; pos--) {
        meuServo.setPosicao(pos);
        delay(15); // Aguarda 15ms
    }
}

```

### Com ESP-IDF

```cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <ServoMB.h>

// Instancia o servo no pino 18, utilizando o canal PWM 0
ServoMB meuServo(18, 0);

extern "C" void app_main() {
    while (true) {
        // Varredura de 0 a 180 graus
        for (int pos = 0; pos <= 180; pos++) {
            meuServo.setPosicao(pos);
            vTaskDelay(15 / portTICK_PERIOD_MS); // Aguarda 15ms
        }

        // Varredura de 180 a 0 graus
        for (int pos = 180; pos >= 0; pos--) {
            meuServo.setPosicao(pos);
            vTaskDelay(15 / portTICK_PERIOD_MS); // Aguarda 15ms
        }
    }
}

```

> [Outros exemplos de uso aqui](https://github.com/MinervaBots/ServoMB/tree/main/examples)

---

## 🦉 Sobre a MinervaBots

A **MinervaBots** é a equipe de competição de robótica da Universidade Federal do Rio de Janeiro (UFRJ). Desenvolvemos tecnologias, robôs autônomos e de combate, além de fomentar a pesquisa e o ensino de engenharia no Brasil.

[![Gmail](https://img.shields.io/badge/-minervabots@poli.ufrj.br-D14836?style=for-the-badge&logo=gmail&logoColor=white&link=mailto:engdanilofroes@gmail)](mailto:minervabots@poli.ufrj.br)

---

**Licença:** MIT
