/** ************************************************************************
 * @file registers.cpp
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup registers
 * @brief Comunication protokol for uart, standard areospace.
 *
 * ************************************************************************* */

#include "registers.h"

namespace Registers
{
    static uint32_t u32_motorMotionMin[6]   = {110, 100, 100, 100, 100, 100};
    static uint32_t u32_motorMotionMax[6]   = {570, 570, 300, 300, 300, 300};
    static int16_t  i32_inputMotor[6];
    static int16_t  i32_inputPosition[6];
    static int16_t  i32_currentPosition[6]  = {150,300,150,150,150,200};
    static bool     b_finnishPosition[6]    = {true,true,true,true,true,true};
    static uint8_t  u8_speedCntMotor[6]     = {10, 10, 10, 10, 10, 10};
    static uint8_t  u8_msDelay = 10;
    static bool     b_motorDisable[6]       = {1, 1, 1, 1, 1, 1};

    static uint8_t u8_EnEeprom = 0;

    static uint16_t u16_AdcRaw[6] = {0U, 0U, 0U, 0U, 0U, 0U};

    static uint16_t u16_PID_SetPoint[4]     = {300U, 0U, 0U, 0U};
    static float    fp_PID_out[4]           = {0.0, 0.0, 0.0, 0.0};
    static float    fp_PID_dt[4]            = {100, 100, 100, 100};
    static float    fp_PID_max[4]           = {1024, 1024, 1024, 1024};
    static float    fp_PID_min[4]           = {-1024, -1024, -1024, -1024};
    static float    fp_PID_Kp[4]            = {1, 1, 1, 1};
    static float    fp_PID_kd[4]            = {0, 0, 0, 0};
    static float    fp_PID_ki[4]            = {0.1, 0.1, 0.1, 0.1}; 
    static uint8_t  u8_PID_EnMsg            = 0;
    static uint8_t  u8_test                 = 0;

    static const DATABASE par_database [] = {
        {	AS_ULONG,	RW,	(void *)&u32_motorMotionMin,	6U,	0U,	0	},
        {	AS_ULONG,	RW,	(void *)&u32_motorMotionMax,	6U,	6U,	24	},
        {	AS_SHORT,	RW,	(void *)&i32_inputMotor,	6U,	12U,	48	},
        {	AS_SHORT,	RW,	(void *)&i32_inputPosition,	6U,	18U,	60	},
        {	AS_SHORT,	RW,	(void *)&i32_currentPosition,	6U,	24U,	72	},
        {	AS_BCHAR,	RW,	(void *)&b_finnishPosition,	6U,	30U,	84	},
        {	AS_UCHAR,	RW,	(void *)&u8_speedCntMotor,	6U,	36U,	90	},
        {	AS_UCHAR,	RW,	(void *)&u8_msDelay,	1U,	42U,	91	},
        {	AS_BCHAR,	RW,	(void *)&b_motorDisable,	6U,	43U,	97	},
        {	AS_UCHAR,	R,	(void *)&u8_EnEeprom,	1U,	49U,	97	},
        {	AS_USHORT,	R,	(void *)&u16_AdcRaw,	6U,	50U,	97	},
        {	AS_USHORT,	R,	(void *)&u16_PID_SetPoint,	4U,	56U,	97	},
        {	AS_FLOAT,	R,	(void *)&fp_PID_out,	4U,	60U,	97	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_dt,	4U,	68U,	113	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_max,	4U,	72U,	129	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_min,	4U,	76U,	145	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_Kp,	4U,	80U,	161	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_kd,	4U,	84U,	177	},
        {	AS_FLOAT,	RW,	(void *)&fp_PID_ki,	4U,	88U,	193	},
        {	AS_UCHAR,	R,	(void *)&u8_PID_EnMsg,	1U,	92U,	193	},
        {	AS_UCHAR,	R,	(void *)&u8_test,	1U,	93U,	193	},
    };

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return uint8_t 
     */
    uint8_t Registers::regGetDataType(TENU_REG_INDEX regIndex)
    {
        return par_database[regIndex].u8_dtype;
    }

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return ENU_MEM_PRT 
     */
    ENU_MEM_PRT Registers::regGetRWstatus(TENU_REG_INDEX regIndex)
    {
        return par_database[regIndex].enu_r_rw;
    }

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return void* 
     */
    void * Registers::regGetPointer(TENU_REG_INDEX regIndex)
    {        
        return par_database[regIndex].v_var;
    }

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return uint8_t 
     */
    uint8_t Registers::regGetRegSize(TENU_REG_INDEX regIndex)
    {
        return par_database[regIndex].u8_reg_size;
    }

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return uint8_t 
     */
    uint32_t Registers::regGetUartAddr(TENU_REG_INDEX regIndex)
    {
        return par_database[regIndex].u32_addr;
    }

    /**
     * @brief 
     * 
     * @param regIndex 
     * @return uint8_t 
     */
    uint32_t Registers::regGetEepromAddr(TENU_REG_INDEX regIndex)
    {
        return par_database[regIndex].u32_reg;
    }

    /**
     * @brief 
     * 
     * @return uint32_t 
     */
    uint32_t Registers::regGetSizeDatabase()
    {
        return (sizeof(par_database)/ sizeof(par_database[0]));
    }
}