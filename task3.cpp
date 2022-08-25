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
        APAGADO,
        ENCENDIDO,
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
        keyCounter = 0;
        taskState = TaskStates::WAIT_CONFIG;
        break;
    }
    case TaskStates::WAIT_CONFIG:
    {

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BUTTON)
            {
                if (bombCounter < 60)
                    bombCounter++;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BUTTON)
            {
                if (bombCounter > 10)
                    bombCounter--;
            }
            else if (buttonEvt.whichButton == BUTTONS::ARM_BTN)
            {
                initLedCounterTimer = millis();
                initBombTimer = millis();
                keyCounter = 0;
                taskState = TaskStates::COUNTING;
            }
            Serial.print("Counter: ");
            Serial.print(bombCounter);
            Serial.print("\n");
        }

        break;
    }
    case TaskStates::COUNTING:
    {

        uint32_t timeNow = millis();

        if ((timeNow - initBombTimer) > BOMBINTERVAL)
        {
            initBombTimer = timeNow;
            bombCounter--;
            Serial.print("Counter: ");
            Serial.print(bombCounter);
            Serial.print("\n");
            if (bombCounter == 0)
            {
                ledBombCountingState = HIGH;
                Serial.print("BOMB BOOM\n");
                digitalWrite(ledBombBoom, HIGH);
                delay(2000);
                digitalWrite(ledBombBoom, LOW);
                digitalWrite(ledBombCounting, ledBombCountingState);
                bombCounter = 20;
                taskState = TaskStates::WAIT_CONFIG;
            }
        }
        if ((timeNow - initLedCounterTimer) > LEDCOUNTERINTERVAL)
        {
            initLedCounterTimer = timeNow;
            ledBombCountingState = !ledBombCountingState;
            digitalWrite(ledBombCounting, ledBombCountingState);
        }

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            disarmKey[keyCounter] = buttonEvt.whichButton;
            keyCounter++;
            if (keyCounter == 7)
            {
                keyCounter = 0;
                if (compareKeys(secret, disarmKey) == true)
                {
                    ledBombCountingState = HIGH;
                    digitalWrite(ledBombCounting, ledBombCountingState);
                    Serial.print("BOMB DISARM\n");
                    bombCounter = 20;
                    taskState = TaskStates::WAIT_CONFIG;
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