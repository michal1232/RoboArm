/** ************************************************************************
 * @file eeprom.cpp
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
#include "eeprom_drv.h"

/**
 * @brief This namespace contain function for reading and writing data to eeprom.
 * 
 */
namespace eepromNs
{
    
    Registers::Registers reg;
    /**
     * @brief 
     * 
     */
    void eepromCls::writeAll()
    { 
        uint32_t u32_database_size = reg.regGetSizeDatabase();
        //Serial.print("Database size: "); Serial.println(u32_database_size);
        for(uint32_t i = 0U; i < u32_database_size; i++)
        {
            if(reg.regGetRWstatus((TENU_REG_INDEX)i) == Registers::RW)
            {
                uint8_t u8_reg_size = reg.regGetRegSize((TENU_REG_INDEX)i);
                uint8_t u8_data_type = reg.regGetDataType((TENU_REG_INDEX)i);
                uint32_t u32_eprom_addr = reg.regGetEepromAddr((TENU_REG_INDEX)i);

                float *fp_data;
                int32_t *i32_data;
                uint32_t *u32_data;
                int16_t *i16_data;
                uint16_t *u16_data;
                int8_t *i8_data;
                uint8_t *u8_data;

                //Serial.print("Index: "); Serial.println(i);
                //Serial.print("Reg size: "); Serial.println(u8_reg_size);
                //Serial.print("Eeprom addr: "); Serial.println(u32_eprom_addr);

                for(uint8_t j = 0U; j < u8_reg_size; j++)
                {
                    //Serial.print("Order: "); Serial.println(j);
                    switch (u8_data_type)
                    {
                    case AS_FLOAT:
                        fp_data = (float *)reg.regGetPointer((TENU_REG_INDEX)i);
                        break;
                    case AS_LONG:
                        i32_data = (int32_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(i32_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(i32_data[j]&0xFF));
                        EEPROM.write(u32_eprom_addr+1U, (uint8_t)((i32_data[j]>>8U)&0xFF));
                        EEPROM.write(u32_eprom_addr+2U, (uint8_t)((i32_data[j]>>16U)&0xFF));
                        EEPROM.write(u32_eprom_addr+3U, (uint8_t)((i32_data[j]>>24U)&0xFF));
                        u32_eprom_addr +=4U;
                        break;
                    case AS_ULONG:
                        u32_data = (uint32_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(u32_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(u32_data[j]&0xFF));
                        EEPROM.write(u32_eprom_addr+1U, (uint8_t)((u32_data[j]>>8U)&0xFF));
                        EEPROM.write(u32_eprom_addr+2U, (uint8_t)((u32_data[j]>>16U)&0xFF));
                        EEPROM.write(u32_eprom_addr+3U, (uint8_t)((u32_data[j]>>24U)&0xFF));
                        u32_eprom_addr +=4U;
                        break;
                    case AS_SHORT:
                        i16_data = (int16_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(i16_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(i16_data[j]&0xFF));
                        EEPROM.write(u32_eprom_addr+1U, (uint8_t)((i16_data[j]>>8U)&0xFF));
                        u32_eprom_addr +=2U;
                        break;
                    case AS_USHORT:
                        u16_data = (uint16_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(u16_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(u16_data[j]&0xFF));
                        EEPROM.write(u32_eprom_addr+1U, (uint8_t)((u16_data[j]>>8U)&0xFF));
                        u32_eprom_addr +=2U;
                        break;
                    case AS_CHAR:
                        i8_data = (int8_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(i8_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(i8_data[j]&0xFF));
                        u32_eprom_addr++;
                        break;
                    case AS_UCHAR:
                        u8_data = (uint8_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        //Serial.print("Data: "); Serial.println(u8_data[j]);
                        EEPROM.write(u32_eprom_addr, (uint8_t)(u8_data[j]&0xFF));
                        u32_eprom_addr++;
                        break;
                    
                    default:
                        break;
                    }
                }
            }
        }
    }

    void eepromCls::readAll()
    {
        uint32_t u32_database_size = reg.regGetSizeDatabase();

        for(uint32_t i = 0U; i < u32_database_size; i++)
        {
            if(reg.regGetRWstatus((TENU_REG_INDEX)i) == Registers::RW)
            {
                uint8_t u8_reg_size = reg.regGetRegSize((TENU_REG_INDEX)i);
                uint8_t u8_data_type = reg.regGetDataType((TENU_REG_INDEX)i);
                uint32_t u32_eprom_addr = reg.regGetEepromAddr((TENU_REG_INDEX)i);

                float *fp_data;
                int32_t *i32_data;
                uint32_t *u32_data;
                int16_t *i16_data;
                uint16_t *u16_data;
                int8_t *i8_data;
                uint8_t *u8_data;

                for(uint8_t j = 0U; j < u8_reg_size; j++)
                {
                    switch (u8_data_type)
                    {
                    case AS_FLOAT:
                        fp_data = (float *)reg.regGetPointer((TENU_REG_INDEX)i);
                        break;
                    case AS_LONG:
                        i32_data = (int32_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        i32_data[j] = 0U;
                        i32_data[j] |= EEPROM.read(u32_eprom_addr);
                        i32_data[j] |= EEPROM.read(u32_eprom_addr+1U)<<8U;
                        i32_data[j] |= EEPROM.read(u32_eprom_addr+2U)<<16U;
                        i32_data[j] |= EEPROM.read(u32_eprom_addr+3U)<<24U;
                        u32_eprom_addr +=4U;
                        break;
                    case AS_ULONG:
                        u32_data = (uint32_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        u32_data[j] = 0U;
                        u32_data[j] |= EEPROM.read(u32_eprom_addr);
                        u32_data[j] |= EEPROM.read(u32_eprom_addr+1U)<<8U;
                        u32_data[j] |= EEPROM.read(u32_eprom_addr+2U)<<16U;
                        u32_data[j] |= EEPROM.read(u32_eprom_addr+3U)<<24U;
                        u32_eprom_addr +=4U;
                        break;
                    case AS_SHORT:
                        i16_data = (int16_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        i16_data[j] = 0U;
                        i16_data[j] |= EEPROM.read(u32_eprom_addr);
                        i16_data[j] |= EEPROM.read(u32_eprom_addr+1U)<<8U;
                        u32_eprom_addr +=2U;
                        break;
                    case AS_USHORT:
                        u16_data = (uint16_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        u16_data[j] = 0U;
                        u16_data[j] |= EEPROM.read(u32_eprom_addr);
                        u16_data[j] |= EEPROM.read(u32_eprom_addr+1U)<<8U;
                        u32_eprom_addr +=2U;
                        break;
                    case AS_CHAR:
                        i8_data = (int8_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        i8_data[j] |= EEPROM.read(u32_eprom_addr);
                        u32_eprom_addr++;
                        break;
                    case AS_UCHAR:
                        u8_data = (uint8_t *)reg.regGetPointer((TENU_REG_INDEX)i);
                        u8_data[j] |= EEPROM.read(u32_eprom_addr);
                        u32_eprom_addr++;
                        break;
                    
                    default:
                        break;
                    }
                }
            }
        }
    }
}