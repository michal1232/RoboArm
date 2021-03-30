/** ************************************************************************
 * @file
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/17/10 17:00, Michal Veteska
 *
 * @addtogroup IdData
 * @brief The module contains version of project RooRuka.
 *
 * ************************************************************************* */
#ifndef __IDDATA_H__
#define __IDDATA_H__

#include "Arduino.h"

#define SW_VERSION "2.0.0"
#define HW_VERSION "2.0.0"
#define MCH_VERSION "1.0.0"

typedef enum {
    GET = 0,
    SET = 1, 
}TENU_GETSET;

/** Enum of index in database.*/
typedef enum {
    U32_MOTORMOTIONMIN	 = 0U,
    U32_MOTORMOTIONMAX	 = 1U,
    I32_INPUTMOTOR	     = 2U,
    I32_INPUTPOSITION	 = 3U,
    I32_CURRENTPOSITION	 = 4U,
    B_FINNISHPOSITION	 = 5U,
    U8_SPEEDCNTMOTOR	 = 6U,
    U8_MSDELAY	         = 7U,
    B_MOTORDISABLE	     = 8U,
    U8_ENEEPROM	         = 9U,
    U16_ADCRAW	         = 10U,
    U16_PID_SETPOINT	 = 11U,
    FP_PID_OUT	         = 12U,
    FP_PID_DT	         = 14U,
	FP_PID_MAX	         = 15U,
	FP_PID_MIN	         = 16U,
	FP_PID_KP	         = 17U,
	FP_PID_KD	         = 18U,
	FP_PID_KI	         = 19U,
    U8_PID_ENMSG	     = 19U,
}TENU_REG_INDEX;

#endif /* __IDDATA_H__ */