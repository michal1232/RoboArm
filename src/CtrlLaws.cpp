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
#include "CtrlLaws.h"

namespace CtrlLawsNS
{

    uint32_t *u32_motor_motion_min;
    uint32_t *u32_motor_motion_max;

    int *i32_inputMotor;
    int *i32_inputPosition;
    int *i32_currentPosition;
    bool *b_finnishPosition;

    uint8_t *u8_speedCntMotor;

    uint8_t *u8_msDelay;

    bool *b_motorDisable;


    /**
     * @brief Construct a new Ctrl Laws Cls:: Ctrl Laws object
     * 
     * @param Driver 
     */
    CtrlLawsCls::CtrlLawsCls(Adafruit_PWMServoDriver &ExDriver)
    {
        pwmDriver = &ExDriver;
    }

    /**
     * @brief 
     * 
     * @param u8_sequenceCounter 
     * @return true 
     * @return false 
     */
    bool CtrlLawsCls::CtrlMove(uint8_t u8_sequenceCounter)
    {
        u32_motor_motion_min = (uint32_t *)reg.regGetPointer(U32_MOTORMOTIONMIN);
        u32_motor_motion_max = (uint32_t *)reg.regGetPointer(U32_MOTORMOTIONMAX);
        i32_inputMotor = (int16_t *)reg.regGetPointer(I32_INPUTMOTOR);
        i32_inputPosition = (int16_t *)reg.regGetPointer(I32_INPUTPOSITION);
        i32_currentPosition  = (int16_t *)reg.regGetPointer(I32_CURRENTPOSITION);
        b_finnishPosition  = (bool *)reg.regGetPointer(B_FINNISHPOSITION);
        u8_speedCntMotor  = (uint8_t *)reg.regGetPointer(U8_SPEEDCNTMOTOR);
        u8_msDelay = (uint8_t *)reg.regGetPointer(U8_MSDELAY);
        b_motorDisable  = (bool *)reg.regGetPointer(B_MOTORDISABLE);

      for(uint8_t i = 0; i <= u8_sequenceCounter; i++)
      {
        if(b_finnishPosition[i] == false)
        {
          if(b_motorDisable[i32_inputMotor[i]] == true)
          {
              b_finnishPosition[i] = true;
              pwmDriver->setPWM(i32_inputMotor[i], 0, 0);  
          }
          else if(u32_motor_motion_min[i32_inputMotor[i]] <= (uint32_t)i32_inputPosition[i] && 
             u32_motor_motion_max[i32_inputMotor[i]] >= (uint32_t)i32_inputPosition[i] )
          {
             if(i32_inputPosition[i] > i32_currentPosition[i32_inputMotor[i]])
             {
               if(u8_speedCntMotor[i32_inputMotor[i]] >= (i32_inputPosition[i] - i32_currentPosition[i32_inputMotor[i]]))
               {
                i32_currentPosition[i32_inputMotor[i]] = i32_inputPosition[i];
                b_finnishPosition[i] = true;
               }
               else
               {
                i32_currentPosition[i32_inputMotor[i]] += u8_speedCntMotor[i32_inputMotor[i]];
               }
               
             }
             else if(i32_inputPosition[i] < i32_currentPosition[i32_inputMotor[i]])
             {
               if(u8_speedCntMotor[i32_inputMotor[i]] >= (i32_currentPosition[i32_inputMotor[i]] - i32_inputPosition[i]))
               {
                i32_currentPosition[i32_inputMotor[i]] = i32_inputPosition[i];
                b_finnishPosition[i] = true;
               }
               else
               {
                i32_currentPosition[i32_inputMotor[i]] -= u8_speedCntMotor[i32_inputMotor[i]];
               }
             }
             else
             {
                b_finnishPosition[i] = true;
             }
             
            pwmDriver->setPWM(i32_inputMotor[i], 0, i32_currentPosition[i32_inputMotor[i]]);
          } 

          else
          {
              b_finnishPosition[i] = true;  
          }
        }

      }
      if(b_finnishPosition[0] == true && b_finnishPosition[1] == true &&
         b_finnishPosition[2] == true && b_finnishPosition[3] == true &&
         b_finnishPosition[4] == true && b_finnishPosition[5] == true)
        {
            for(uint8_t i = 0; i < 6; i++)
            {
              b_finnishPosition[i] = true;
              //u8_sequenceCounter = 0;
            }
            return true;
        }
      return false; 
    }
    
}