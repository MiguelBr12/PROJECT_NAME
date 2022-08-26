#include <Arduino.h>
#include "task3.h"
#include "events.h"

static bool compareKeys(BUTTONS *pSecret, BUTTONS *pKey)
{
    bool correct = true;
    for (uint8_t i = 0; i < 5; i++)
    {
        if (pSecret[i] != pKey[i])
        {
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
        LENTO,
        MEDIO,
        RAPIDO,
        PERMANENTEMENTE_APAGADO,
        PERMANENTEMENTE_ENCENDIDO,
        ESPERAR_ENCENDIDO,
        ESPERAR_APAGADO,
    };
    static TaskStates taskState = TaskStates::INIT;

    static uint8_t ledBlue = 26; // Declaro la posición de la led

    static uint32_t lasTime;
    static constexpr uint32_t SLOW_TIME = 500;   // Cantidad de tiempo que se demora
    static constexpr uint32_t MEDIUM_TIME = 250; // Cantidad de tiempo que se demora
    static constexpr uint32_t FAST_TIME = 125;   // Cantidad de tiempo que se demora

    static bool ledStatus = false;
    static bool pregunta_apagado = false; // Variable para preguntar si la led esta apagada o si la led estaba prendida

    static BUTTONS secret[5] = {BUTTONS::ONE_BTN, BUTTONS::ONE_BTN,
                                BUTTONS::TWO_BTN, BUTTONS::TWO_BTN,
                                BUTTONS::ONE_BTN}; // Codigo para salir del modo rapido

    static BUTTONS disarmKey[5] = {BUTTONS::NONE};

    static uint8_t keyCounter;

    switch (taskState)
    {
    case TaskStates::INIT:
    {

        pinMode(ledBlue, OUTPUT);
        digitalWrite(ledBlue, LOW);

        taskState = TaskStates::LENTO;
        break;
    }

    case TaskStates::LENTO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= SLOW_TIME)
        {
            lasTime = currentTime;
            ledStatus = !ledStatus;
            digitalWrite(ledBlue, ledStatus);
            
        }

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::ESPERAR_APAGADO;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                taskState = TaskStates::MEDIO;
            }
        }

        break;
    }

    case TaskStates::ESPERAR_APAGADO:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= SLOW_TIME)
        {
            ledStatus = false;
        digitalWrite(ledBlue, ledStatus);
            taskState = TaskStates::PERMANENTEMENTE_APAGADO;
        }
    }
    case TaskStates::PERMANENTEMENTE_APAGADO:
    {
         
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                
                
                taskState = TaskStates::LENTO;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                pregunta_apagado = true;
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
        }
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::ESPERAR_ENCENDIDO;
                    
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                taskState = TaskStates::LENTO;
            }
        }
        

        break;
    }

    case TaskStates::ESPERAR_ENCENDIDO:
    {
        uint32_t currentTime = millis();
         if ((currentTime - lasTime) >= MEDIUM_TIME)
         {
            ledStatus = true;
            digitalWrite(ledBlue, ledStatus);
            taskState = TaskStates::PERMANENTEMENTE_ENCENDIDO;
         }
       
        
    }

    case TaskStates::PERMANENTEMENTE_ENCENDIDO:
    {
        
        
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::MEDIO;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                pregunta_apagado = false;
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
            
            ledStatus = !ledStatus;
            digitalWrite(ledBlue, ledStatus);
            
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

                    Serial.print("Regresar al modo anterior\n");

                    if (pregunta_apagado == true)
                    {
                        taskState = TaskStates::PERMANENTEMENTE_APAGADO; // Si estaba apagado, regresa a estar apagado
                    }
                    else
                    {
                        taskState = TaskStates::PERMANENTEMENTE_ENCENDIDO; // Si estaba prendido, regresa a estar prendido
                    }
                }
                else
                {
                    Serial.print("Lo lamento, te equivocaste.\n");
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