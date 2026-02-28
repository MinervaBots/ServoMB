
#include <Arduino.h>
#include <ServoMB.h>

constexpr uint8_t PINO_SERVO = 18;
constexpr uint8_t CANAL_SERVO = 0;

ServoMB servo(PINO_SERVO, CANAL_SERVO);

void setup() {}

void loop() {
    for (uint8_t posicao = 0; posicao <= 180; posicao++) {
        servo.setPosicao(posicao);
        delay(15);
    }

    for (uint8_t posicao = 180; posicao > 0; posicao--) {
        servo.setPosicao(posicao);
        delay(15);
    }
}