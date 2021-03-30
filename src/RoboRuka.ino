// Driver servomotorů PC9685

// připojení potřebných knihoven
#include <Wire.h>
#include <EEPROM.h>

#include "Adafruit_PWMServoDriver.h"
#include "task.h"

#include "CtrlLaws.h"
#include "rtiHandler.h"
#include "PidReg.h"
#include "adc.h"
#include "uartaero.h"
#include  "eeprom_drv.h"
#include "registers.h"

/* *******************************************************************************
 * Using namespace.
 * *******************************************************************************/
using namespace CtrlLawsNS;
using namespace eepromNs;
using namespace RtiNS;
using namespace taskNS;
using namespace AdcNS;
using namespace UartAreo;
using namespace Registers;

/* *******************************************************************************
 * Local variale and object.
 * *******************************************************************************/
void SerialAdamTx(uint16_t  id, uint8_t  len, uint8_t *frame);

#define PWM_DRIVER
// vytvoření objektu driveru z knihovny
#ifdef PWM_DRIVER
Adafruit_PWMServoDriver driverPCA = Adafruit_PWMServoDriver(0x40);
CtrlLawsCls driveRuka(driverPCA);
#else
CtrlLawsCls driveRuka;
#endif
eepromCls eepromService;
taskCls TaskSheduler;
PidReg* pidReg = (PidReg*)malloc(sizeof(PidReg) * 4); 
AdcCls adcIsr;

String SerialAdamStr ="";
UartAero SerialAdam(SerialAdamTx, SerialAdamStr);
Registers::Registers reg;

/* *******************************************************************************
 * Arduino setup and loop
 * *******************************************************************************/

void setup() {
  sei();
  // zahájení komunikace po sériové lince
  Serial.begin(115200);
  // reserve 200 bytes for the SerialAdamStr:
  SerialAdamStr.reserve(50);

  //eepromService.readAll();

#ifdef PWM_DRIVER
  // zahájení komunikace s driverem
  driverPCA.begin();
  // nastavení výchozí PWM frekvence na 60Hz
  driverPCA.setPWMFreq(100);
  delay(10);

  int16_t *i16_started_position = (int16_t *)reg.regGetPointer(I32_CURRENTPOSITION);
  int16_t *i16_input_position = (int16_t *)reg.regGetPointer(I32_INPUTPOSITION);
  bool *b_motor_disale = (bool *)reg.regGetPointer(B_MOTORDISABLE);

  for(uint8_t i = 0; i < 6; i++)
  {
    if(b_motor_disale[i] != 1)
    {
      driverPCA.setPWM(i, 0, i16_started_position[i]);
      delay(10);
    }
    else
    {
      driverPCA.setPWM(i, 0, 0);
    }
    i16_input_position[i] = i16_started_position[i];
  }
#endif

  v_init_tasklist();  

  rti0.rti0Handler(TimerHandler);
  rti0.start();
  adcIsr.AdcClsInit();

  for (uint8_t i = 0; i < 4; i++) { 
        pidReg[i] = PidReg(100, 1023, -1023, 0.8, 0.0, 0.005); 
    } 
  
}

/**
 * @brief 
 * 
 */
void loop() {

  TaskSheduler.xv_task_chk_one_pass();

}

/* *******************************************************************************
 * Interrupt handler
 * *******************************************************************************/
/**
 * @brief 
 * 
 */
void TimerHandler(void)
{
  TaskSheduler.xv_task_timeinc();
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    SerialAdamStr += inChar;
  } 
}



void SerialAdamTx(uint16_t  id, uint8_t  len, uint8_t *frame)
{

}

/* *******************************************************************************
 * Task list variale, function and procedure
 * *******************************************************************************/
struct task xs_task_CtrlLaw     = {CtrlLaw_task, 10, 0, false};
struct task xs_task_eepromService  = {EepromService_task, 100, 0, true};
struct task xs_task_PidReg      = {PidReg_task, 100, 1, true};
struct task xs_task_SeralAdam   = {SeralAdam_task, 25, 2, true};


static struct task *gs_tasklist[] =
{
    &xs_task_CtrlLaw,
    &xs_task_eepromService,
    &xs_task_PidReg,
    &xs_task_SeralAdam
};

void v_init_tasklist(void)
{
  TaskSheduler.xv_task_ini(gs_tasklist, 4);
}

/* *******************************************************************************
 * Task function run in sheduler
 * *******************************************************************************/
/**
 * @brief 
 * 
 */
void CtrlLaw_task(void)
{
  uint8_t *u8_msDelay = (uint8_t *)reg.regGetPointer(U8_MSDELAY);
  TaskSheduler.xv_task_set_period(&xs_task_CtrlLaw, *u8_msDelay);

  /*if(scanMsg.sequenceEndGetSet(false, GET) == true)
  {
      if(driveRuka.CtrlMove(scanMsg.sequenceCounterGetSet(0, GET)) == true)
      {
        scanMsg.sequenceCounterGetSet(0, SET);
        scanMsg.sequenceEndGetSet(false, SET);
      }
  }*/
}

void SeralAdam_task(void)
{
  if(SerialAdamStr != "")
  {
    //SerialAdam.UartAeroRxProc();
    Serial.println(SerialAdamStr);
    SerialAdamStr = "";
  }
}

/**
 * @brief 
 * 
 */
void EepromService_task(void)
{
  uint8_t *u8_EnEepromBF = (uint8_t *)reg.regGetPointer(U8_ENEEPROM);

  if((u8_EnEepromBF[0]&0x01) == 1U) {
    eepromService.writeAll();
    u8_EnEepromBF[0] = 0U;
  } 
  if(((u8_EnEepromBF[0]>>1U)&0x01) == 1U) {
    eepromService.readAll();
    u8_EnEepromBF[0] = 0U;
  } 
}


/**
 * @brief 
 * 
 */
void PidReg_task(void)
{
  if(true)
  {
    uint16_t *adcArray = (uint16_t *)reg.regGetPointer(U16_ADCRAW);
    uint16_t * u16_sp = (uint16_t *)reg.regGetPointer(U16_PID_SETPOINT);
    float *fp_pidOut = (float *)reg.regGetPointer(FP_PID_OUT);
    uint8_t *u8_enSend = (uint8_t *)reg.regGetPointer(U8_PID_ENMSG); 
    CANAERO_MSG msg;

    for(uint8_t i = 0; i < 4; i++) {
      fp_pidOut[i] = pidReg[i].PidReg_Calculate(u16_sp[i], adcArray[i]);

      if(((u8_enSend[0]>>i)&0x01) == 1)
      {
        msg.data_type = AS_SHORT_2;
        msg.node_id = 10;
        msg.service_code = 0;
        msg.message_code = 0;
        msg.message_data.int16_2[0] = (uint16_t)fp_pidOut[i];
        msg.message_data.int16_2[0] = adcArray[i];
        SerialAdam.UartAeroTxPacket(1000+i, 8, &msg);
      }
    }

    


    /*if(pidOut > 0)
    {
      fp32_motorDir1 = 4.0*pidOut;
    }
    else
    {
      fp32_motorDir1 = (-4.0)*pidOut; 
    }*/
  }
}
