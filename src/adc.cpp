/** ************************************************************************
 * @file adc.cpp
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup Adc
 * @brief The module contains functions that drive core of software roboruka.
 *
 * ************************************************************************* */
#include "adc.h"

namespace AdcNS
{
    uint16_t *gu16_adc_result;
    static uint8_t gu8_adc_pin = 0;

    Registers::Registers reg;

    /**
     * @brief 
     * 
     */
    void AdcCls::AdcClsInit() {
        pinMode(A0, INPUT);
        // clear ADLAR in ADMUX (0x7C) to right-adjust the result
        // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
        ADMUX &= ~(1 << ADLAR);

        //Referenc voltage AVCC with external capacitor at AREF pin.
        ADMUX |= (1 << REFS0);

        // Set ADEN in ADCSRA (0x7A) to enable the ADC.
        // Note, this instruction takes 12 ADC clocks to execute
        ADCSRA |= (1 << ADEN);
      
        // Clean ADATE in ADCSRA (0x7A) to enable auto-triggering.
        ADCSRA &= ~(1 << ADATE);

        // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
        // This means that as soon as an ADC has finished, the next will be
        // immediately started.
        //ADCSRB &= B11111000;
        
        // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
        // Above 200KHz 10-bit results are not reliable.
        ADCSRA |= B00000111;

        // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
        // Without this, the internal interrupt will not trigger.
        ADCSRA |= (1 << ADIE);
     
        //First start
        ADMUX |= (gu8_adc_pin & 0x07);
        ADCSRA |= (1 << ADSC); // start the conversion

        gu16_adc_result = (uint16_t *)reg.regGetPointer(U16_ADCRAW);
    }

    /**
     * @brief 
     * 
     */
    uint16_t *AdcCls::GetAdc() {
        //return gu16_adc_result;
    } 

    /**
     * @brief 
     * 
     */
    ISR(ADC_vect) {  
        gu16_adc_result[gu8_adc_pin] = ADCL | (ADCH << 8);
        gu8_adc_pin++;
        if (gu8_adc_pin == reg.regGetRegSize(U16_ADCRAW)) {
            gu8_adc_pin = 0;
        }
        ADMUX &= 0b11111000;
        ADMUX |= (gu8_adc_pin & 0x07);
        ADCSRA |= (1 << ADSC); // start the conversion       
    }
}