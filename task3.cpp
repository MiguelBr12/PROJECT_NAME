#include <Arduino.h>
#include "task3.h"

static bool compareKeys(BUTTONS *pSecret, BUTTONS *pKey)
{
    bool correct = true;
    for (uint8_t i = 0; i < 5; i++)
    {
        if (pSecret[i] != pKey[i]){
            correct = false;
            break;
        }
    }

    return correct;
}

void task3()
{

    enum class TaskStates
    {
        INIT,
        WAIT_CONFIG,
        lENTO,
        MEDIO,
        RAPIDO,
        PERMANENTEMENTE_APAGADO,
        PERMANENTEMENTE_ENCENDIDO,
        ESPERAR_A_APAGADO, // Estado para pasar de modo, esperando a que termine de apagarse
        ESPERAR_A_ENCENDIDO, // Estado para pasar de modo, esperando a que termine de encenderse
    };
    static TaskStates taskState = TaskStates::INIT;

    static uint8_t ledBlue = 26; //Declaro la posición de la led

    static uint32_t lasTime;
    static constexpr uint32_t SLOW_TIME = 500;
    static constexpr uint32_t MEDIUM_TIME = 250;
    static constexpr uint32_t FAST_TIME = 125;

    static bool ledStatus = false;
    static bool pregunta_apagado = false; //Variable para preguntar si la led esta apagada

   
    static BUTTONS secret[5] = {BUTTONS::ONE_BUTTON, BUTTONS::ONE_BUTTON,
                                BUTTONS::TWO_BUTTON, BUTTONS::TWO_BUTTON,
                                BUTTONS::ONE_BUTTON, 
                                };

    static BUTTONS disarmKey[5] = {BUTTONS::NONE};

    static uint8_t keyCounter; 

    switch (taskState)
    {
    case TaskStates::INIT:
    {

        pinMode(ledBlue, OUTPUT)
        digitalWrite(ledBlue, LOW);
       
        taskState = TaskStates::LENTO;
        break;
    }
    case TaskStates::LENTO:
    {
        uint32_t currentTime = millis ();
        if ((currentTime - lasTime) >= SLOW_TIME)
        {
            lasTime = currentTime;
            digitalWrite(ledBlue, ledStatus);
            ledStatus = !ledStatus;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
                {
                    taskState = TaskStates::PERMANENTEMENTE_APAGADO;
                }
                else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
                {
                    taskState = TaskStates::MEDIO;
                }
            }
        }

        break;
    }
    case TaskStates::APAGADO:
    {
        digitalWrite(ledBlue, ledStatus);
        ledStatus = false;
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BUTTON)
            {
                taskState = TaskStates::LENTO;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                Pregunta_Apagado = true;
                taskState = TaskStates::RAPIDO;
            }
        }

        break;
    }

    case TaskStates::MEDIO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= MEDIUM_TIME)
        {
            lasTime = currentTime;
            digitalWrite(ledBlue, ledStatus);
            ledStatus = !ledStatus;
            if (buttonEvt.trigger == true)
            {
                buttonEvt.trigger = false;
                if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
                {
                    taskState = TaskStates::PERMANENTEMENTE_ENCENDIDO;
                }
                else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
                {
                    taskState = TaskStates::LENTO;
                }
            }
        }

        break;
    }
    case TaskStates::ENCENDIDO:
    {
        digitalWrite(ledBlue, ledStatus);
        ledStatus = true;
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::MEDIO;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                Pregunta_Apagado = false;
                taskState = TaskStates::RAPIDO;
            }
        }

        break;
    }
    case TaskStates::RAPIDO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= FAST_TIME)
        {
            lasTime = currentTime;
            digitalWrite(ledBlue, ledStatus);
            ledStatus = !ledStatus;
        }
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            disarmKey[keyCounter] = buttonEvt.whichButton;
            keyCounter++;
            if (keyCounter == 5)
            {
                keyCounter = 0;
                if (compareKeys(secret, disarmKey) == true)
                {

                    Serial.print("salir de modo rápido\n");

                    if (Pregunta_Apagado == true)
                    {
                        taskState = TaskStates::APAGADO;
                    }
                    else
                    {
                        taskState = TaskStates::PERMANENTEMENTE_ENCENDIDO;
                    }
                }
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }
}