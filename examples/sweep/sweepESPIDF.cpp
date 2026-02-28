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