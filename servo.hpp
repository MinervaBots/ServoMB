/*
Cabeçalho responsável pelo controle do servo motor.
*/

#pragma once

#include <Arduino.h>

// Definição do valor da frequência do sinal PWM do servo motor
#define FREQUENCIA_DO_SINAL_PWM_DO_SERVO_MOTOR 50

// Definição da resolução do sinal PWM
#define RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR 15

// Definição das porcentagens do ciclo de trabalho do sinal PWM
// Período do sinal PWM: 20000us
// Valor mínimo do ciclo de trabalho: 500us
// Valor médio do ciclo de trabalho: 1450us
// Valor máximo do ciclo de trabalho: 2400us
#define PORCENTAGEM_MINIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR 0.025  // Servo motor na posição 0°
#define PORCENTAGEM_MEDIA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR  0.0725 // Servo motor na posição 90°
#define PORCENTAGEM_MAXIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR 0.12   // Servo motor na posição 180°

// Constantes com os valores do ciclo de trabalho do sinal PWM
const int VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = (int)((pow(2, RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR) - 1) * PORCENTAGEM_MINIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);
const int VALOR_MEDIO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR  = (int)((pow(2, RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR) - 1) * PORCENTAGEM_MEDIA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);
const int VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = (int)((pow(2, RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR) - 1) * PORCENTAGEM_MAXIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);

// Classe para controlar servo motores
class Servo {
    private:

    // Atributo que armazena o canal utilizado pelo pino de sinal do servo motor
    // Esse canal é responsável por gerar o sinal PWM
    int _canal;

    // Atributo que armazena o valor da posição atual do servo motor
    int _posicao = 0;

    public:
    
    /**
     * @brief Método construtor que configura o servo motor.
     * 
     * @param pino Número do pino de sinal do servo motor;
     * @param canal Canal de PWM do pino de sinal do servo motor.
     */
    Servo(int pino, int canal):
        // Atribui os valores passados nos atributos
        _canal(canal)
    {
        // Configura o canal que gera o sinal PWM
        ledcSetup(_canal, FREQUENCIA_DO_SINAL_PWM_DO_SERVO_MOTOR, RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR);

        // Anexa o canal que gera o sinal PWM no pino de sinal do servo motor
        ledcAttachPin(pino, _canal);
    }

    /**
     * @brief Método para alterar a posição do servo motor de acordo com o valor de posição passado.
     * 
     * @param posicao Valor da posição, na qual varia no intervalo [0, 180], para aplicar no servo motor.
     */
    void setPosicao(int posicao) {
        /*
        Remapeia esse valor do intervalo:
        
        [0, 180]
        
        Para o intervalo:
        
        [VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR, VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR]
        
        E escreve no canal o valor remapeado
        */
        int posicaoRemapeada = map(
            posicao,
            0,
            180,
            VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR,
            VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR
        );

        // Escreve no canal o valor remapeado
        ledcWrite(_canal, posicaoRemapeada);

        // Atualiza o atributo da posição do servo motor com o valor da posição atual
        _posicao = posicao;
    }

    /**
     * @brief Método para obter o valor do atributo que armazena o valor da posição atual do servo motor.
     * 
     * @return O valor da posição atual do servo motor.
     */
    int getPosicao() {
        return _posicao;
    }
};