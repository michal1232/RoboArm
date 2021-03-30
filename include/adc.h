/** ************************************************************************
 * @file adc.h
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup Adc
 * @brief This module periodicli read data from all adc and save it in array
 *
 * ************************************************************************* */

#ifndef __ADC_H__
#define __ADC_H__

#include "Arduino.h"
#include "pins_arduino.h"
#include "registers.h"

namespace AdcNS
{
    class AdcCls
    {
    public:
        void AdcClsInit();
        uint16_t *GetAdc(); 
    private:
       
    };  
}



#endif /*__CTRLLAWS_H__*/