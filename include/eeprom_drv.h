/** ************************************************************************
 * @file eeprom.h
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup eeprom
 * @brief The module contains functions that provide save and read eeporm data.
 *
 * ************************************************************************* */

#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "Arduino.h"
#include "registers.h"
#include <EEPROM.h>

namespace eepromNs
{  
    class eepromCls
    {
    public:
        void writeAll();
        void readAll();
    private:
        Registers::Registers reg;
    };

}

#endif  /*__EEPROM_H__*/