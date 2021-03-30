/** ************************************************************************
 * @file registers.h
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

#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include "Arduino.h"
#include "uartaero.h"
#include "IdData.h"

namespace Registers
{
    typedef enum 
    {
        R = 0,
        W,
        RW,
    }ENU_MEM_PRT;

    typedef struct
    {
        uint8_t    u8_dtype;         /**< CAN Aerospace data type */
        ENU_MEM_PRT    enu_r_rw;          /**< Read only, read/write, null  to MRAM address*/
        void     *v_var;         /**< Pointer to variable */
        uint8_t    u8_reg_size;      /**< Size of register in variable*/
        uint32_t   u32_addr;         /**< MRAM store address */
        uint32_t   u32_reg;          /**< CAN Aerospace register number */
    } DATABASE;

    class Registers
    {
    private:
        
    public:
        uint8_t regGetDataType(TENU_REG_INDEX regIndex);
        ENU_MEM_PRT regGetRWstatus(TENU_REG_INDEX regIndex);
        void * regGetPointer(TENU_REG_INDEX regIndex);
        uint8_t regGetRegSize(TENU_REG_INDEX regIndex);
        uint32_t regGetUartAddr(TENU_REG_INDEX regIndex);
        uint32_t regGetEepromAddr(TENU_REG_INDEX regIndex);
        uint32_t regGetSizeDatabase();
    };
    
}
#endif