#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"
#include "events.h"

buttonEvt_t buttonEvt = {false, BUTTONS::NONE};

void setup()
{
    task1();
    task2();
    task3();
    task4();
}

void loop()
{
    task1();
    task2();
    task3();
    task4();
}
