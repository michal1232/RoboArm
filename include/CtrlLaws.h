/** ************************************************************************
 * @file CtrlLaws.cpp
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup CtrlLaws
 * @brief The module contains functions that drive core of software roboruka.
 *
 * ************************************************************************* */

#ifndef __CTRLLAWS_H__
#define __CTRLLAWS_H__

#include "Arduino.h"
#include "IdData.h"
#include "Adafruit_PWMServoDriver.h"
#include "registers.h"

#define PWM_DRIVER

namespace CtrlLawsNS
{
    class CtrlLawsCls
    {
    public:
#ifdef PWM_DRIVER
        CtrlLawsCls(Adafruit_PWMServoDriver &ExDriver);
#endif
        bool CtrlMove(uint8_t u8_sequenceCounter);

    private:
#ifdef PWM_DRIVER
        Adafruit_PWMServoDriver* pwmDriver;
        Registers::Registers reg;
#endif
    };  
}



#endif /*__CTRLLAWS_H__*/