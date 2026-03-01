/*
Cabeçalho responsável pelo controle do servo motor.
*/

#pragma once

#include "driver/ledc.h"
#include "esp_log.h"

namespace ServoMBConstants {
    // Definição do valor da frequência do sinal PWM do servo motor
    constexpr uint32_t FREQUENCIA_DO_SINAL_PWM_DO_SERVO_MOTOR = 50;

    // Definição da resolução do sinal PWM
    constexpr uint8_t RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR = 15;

    // Definição das porcentagens do ciclo de trabalho do sinal PWM
    // Período do sinal PWM: 20000us
    // Valor mínimo do ciclo de trabalho: 500us
    // Valor médio do ciclo de trabalho: 1450us
    // Valor máximo do ciclo de trabalho: 2400us
    constexpr float PORCENTAGEM_MINIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = 0.025f;  // Servo motor na posição 0°
    constexpr float PORCENTAGEM_MEDIA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR  = 0.0725f; // Servo motor na posição 90°
    constexpr float PORCENTAGEM_MAXIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = 0.12f;   // Servo motor na posição 180°

    constexpr unsigned int VALOR_MAXIMO_RESOLUCAO_PWM_SERVO = (1 << RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR) - 1;

    // Constantes com os valores do ciclo de trabalho do sinal PWM
    constexpr unsigned short int VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = (unsigned short int)(VALOR_MAXIMO_RESOLUCAO_PWM_SERVO * PORCENTAGEM_MINIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);
    constexpr unsigned short int VALOR_MEDIO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR  = (unsigned short int)(VALOR_MAXIMO_RESOLUCAO_PWM_SERVO * PORCENTAGEM_MEDIA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);
    constexpr unsigned short int VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR = (unsigned short int)(VALOR_MAXIMO_RESOLUCAO_PWM_SERVO * PORCENTAGEM_MAXIMA_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR);
}

// Classe para controlar servo motores
class ServoMB {
private:
    // Atributo que armazena o canal utilizado pelo pino de sinal do servo motor
    // Esse canal é responsável por gerar o sinal PWM
    ledc_channel_t _canal;
    uint8_t _pino;

    // Armazenará qual tipo de velocidade de canal será usada
    ledc_mode_t canalVelocidade;

    // Atributo para guardar qual timer foi escolhido dinamicamente
    ledc_timer_t timerSelecionado;

    // Atributo que armazena o valor da posição atual do servo motor
    uint8_t _posicao = 0;

    // Variável para controlar a inicialização
    // Garante com que a parte do hardware seja configurada somente após a inicialização do sistema operacional do ESP32
    bool hardwareIniciado = false;
    bool configurado = false;
    
    /**
     * @brief Método para configurar o canal e o pino do servo motor, além de verificar se os valores passados são válidos.
     * 
     * @param pino Número do pino de sinal do servo motor;
     * @param canal Canal de PWM do pino de sinal do servo motor.
     * 
     * @return true se a configuração for feita com sucesso, ou false caso haja algum erro na configuração.
     */
    bool setConfig(uint8_t pino, uint8_t canal) {
        if (pino > 100) {
            ESP_LOGE("ServoMB", "Pino invalido, o valor passado é muito alto (%d).", pino);
            return false;
        }

        // Macro de verificação do hardware para suporte do HIGH SPEED
        #ifdef SOC_LEDC_SUPPORT_HS_MODE

            if (canal >= 0 && canal <= 7) {
                this->_canal = static_cast<ledc_channel_t>(canal);
                this->canalVelocidade = LEDC_LOW_SPEED_MODE;
            }

            else if (canal > 7 && canal <= 15) {
                // Transforma os canais de 8 a 15 em valores de 0 a 7 para a função de inicialização
                this->_canal = static_cast<ledc_channel_t>(canal - 8);
                // Os canais de 8 a 15 são do hardware HIGH SPEED
                this->canalVelocidade = LEDC_HIGH_SPEED_MODE;
            }

            else {
                ESP_LOGE("ServoMB", "Canal %d invalido para esse chip. Use de 0 a 15.", canal);
                return false;
            }

        #else

            if (canal <= 7) {
                this->_canal = static_cast<ledc_channel_t>(canal);
                this->canalVelocidade = LEDC_LOW_SPEED_MODE;
            } 
            
            else {
                ESP_LOGE("ServoMB", "Canal %d invalido para esse chip. Use de 0 a 7.", canal);
                return false;
            }

        #endif

        this->_pino = pino;
        configurado = true;
        
        return true;
    }

    /**
     * @brief Método para configurar o canal e o timer do ESP32 para o controle do servo motor.
     */
    void initCanal() {
        if (!configurado || hardwareIniciado) { return; }

        bool timerAlocado = false;

        // Inicia um loop para verificar os timers disponíveis e escolher um para configurar o canal do servo motor
        for (uint8_t i = 0; i < LEDC_TIMER_MAX; i++) {
            ledc_timer_t timerTeste = static_cast<ledc_timer_t>(i);

            uint32_t freqAtual = ledc_get_freq(canalVelocidade, timerTeste); // Salva a frequência do timer de teste

            // Se a frequência do timer de teste já for igual a que será usada para o servo motor, então esse timer será reutilizado
            if (freqAtual == ServoMBConstants::FREQUENCIA_DO_SINAL_PWM_DO_SERVO_MOTOR) {
                timerSelecionado = timerTeste;
                timerAlocado = true;
                break; // quebra o loop
            }

            // Se a frequência do timer de teste for 0, significa que ele está livre para ser configurado
            else if (freqAtual == 0) {
                timerSelecionado = timerTeste;

                ledc_timer_config_t ledc_timer = {
                    .speed_mode = canalVelocidade, // Escolhe o modo de velocidade de acordo com o canal de entrada na classe
                    .duty_resolution = static_cast<ledc_timer_bit_t>(ServoMBConstants::RESOLUCAO_DO_SINAL_PWM_DO_SERVO_MOTOR),
                    .timer_num = timerSelecionado,
                    .freq_hz = ServoMBConstants::FREQUENCIA_DO_SINAL_PWM_DO_SERVO_MOTOR,
                    .clk_cfg = LEDC_AUTO_CLK
                };

                ledc_timer_config(&ledc_timer);
                timerAlocado = true;
                break; // quebra o loop
            }
        }

        // Se nenhum timer foi alocado, significa que todos os timers do LEDC estão sendo usados com outras frequências, e o controle do servo motor não pode ser iniciado
        if (!timerAlocado) {
            ESP_LOGE("ServoMB", "Nenhum timer livre! Todos os timers do LEDC estao sendo usados com outras frequencias.");
            return; // Aborta a inicialização do hardware
        }

        ledc_channel_config_t ledc_channel = {
            .gpio_num = _pino, // Pino de saída
            .speed_mode = canalVelocidade, // Escolhe o modo de velocidade de acordo com o canal de entrada na classe
            .channel = _canal, // Canal já remapeado
            .intr_type = LEDC_INTR_DISABLE, // desativa as interrupções
            .timer_sel = timerSelecionado, // Timer já configurado
            .duty = 0, // Começa com o duty cicle em 0
            .hpoint = 0
        };

        ledc_channel_config(&ledc_channel);

        hardwareIniciado = true;
    }

public:

    /**
     * @brief Método construtor que configura o servo motor.
     * 
     * @param pino Número do pino de sinal do servo motor;
     * @param canal Canal de PWM do pino de sinal do servo motor.
     */
    ServoMB(uint8_t pino, uint8_t canal) {
        // Configura as variáveis de pino e canal do objeto e o timer do ESP
        if (!setConfig(pino, canal)) {
            ESP_LOGE("ServoMB", "Falha ao configurar o servo motor. Verifique os valores de pino e canal passados.");
            return;
        }
    }

    /**
     * @brief Método para alterar a posição do servo motor de acordo com o valor de posição passado.
     * 
     * @param posicao Valor da posição, na qual varia no intervalo [0, 180], para aplicar no servo motor.
     */
    void setPosicao(uint8_t posicao) {
        /*
        Remapeia esse valor do intervalo:
        
        [0, 180]
        
        Para o intervalo:
        
        [VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR, VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR]
        
        E escreve no canal o valor remapeado
        */
    if (!configurado) { return; }

        if (!hardwareIniciado) {
            initCanal();
        }

        // Atualiza o atributo da posição do servo motor com o valor da posição atual e limita o valor da posição no intervalo [0, 180]
        _posicao = (posicao > 180) ? 180 : posicao;

        // Remapeia o valor da posição do servo motor para o intervalo do ciclo de trabalho do sinal PWM
        uint32_t diferencaPWM = ServoMBConstants::VALOR_MAXIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR - ServoMBConstants::VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR;
        uint32_t posicaoRemapeada = ServoMBConstants::VALOR_MINIMO_DO_CICLO_DE_TRABALHO_DO_SERVO_MOTOR + ((_posicao * diferencaPWM) / 180);

        // Escreve no canal o valor remapeado
        ledc_set_duty(canalVelocidade, _canal, posicaoRemapeada);
        ledc_update_duty(canalVelocidade, _canal);
    }

    /**
     * @brief Método para obter o valor do atributo que armazena o valor da posição atual do servo motor.
     * 
     * @return O valor da posição atual do servo motor.
     */
    uint8_t getPosicao() const { return _posicao; }

    /**
     * @brief Método para iniciar o controle do servo motor, configurando o canal e o timer do ESP32.
     * Esse método deve ser chamado somente após a inicialização do sistema operacional do ESP32, para garantir que a configuração do hardware seja feita corretamente.
     * Não é necessário chamar esse método manualmente, pois ele é chamado automaticamente no método setPosicao() caso o hardware ainda não tenha sido iniciado,
     * porém, isso fará com que o gasto computacional seja maior no momento de definir a posição do servo motor, já que a configuração do hardware é um processo mais pesado. 
     */
    void begin() { initCanal(); }
};