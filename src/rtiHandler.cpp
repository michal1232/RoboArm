/** ************************************************************************
 * @file rtiHandler.c
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/22/10 22:00, Michal Veteska
 *
 * @addtogroup rtiHandler
 * @brief The module hnadling and setting timer function.
 *
 * ************************************************************************* */
#include "rtiHandler.h"

namespace RtiNS
{
    Rti0Cls rti0;

    void Rti0Cls::start()
    {
        this->rti0Init();
    }

    void Rti0Cls::rti0Handler(void (*function)(void))
    {
        __rti0Handler = function;
    }
    void Rti0Cls::rti0Init()
    {
        TCCR0B = 0;
        TCCR0A = 0;  
        TCNT0 = 0;
         // T/C match it evey 1000ms
        OCR0A = 249;
        // turn on CTC mode
        TCCR0A |= (1 << WGM01);
        // Set CS01 and CS00 bits for 64 prescaler
        TCCR0B |= (1 << CS01) | (1 << CS00);   
        // enable timer compare interrupt
        TIMSK0 |= (1<<OCIE0A); 
    }

    ISR (TIMER0_COMPA_vect)
    {
        //Serial.println("Timer 0");
        __rti0Handler();
    }
}