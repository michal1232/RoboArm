/** ************************************************************************
 * @file rtiHandler.h
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
#ifndef __RTIHANDLER_H__
#define __RTIHANDLER_H__

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

namespace RtiNS
{
    static void (*__rti0Handler)(void);

    class Rti0Cls
    {
        public:
            void start();
            void rti0Handler(void (*function)(void));
        private:
            void rti0Init(void);
    };

    extern Rti0Cls rti0;
}

#endif  /*__RTIHANDLER_H__*/