/** ************************************************************************
 * @file PidReg.h
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup PidReg
 * @brief This module
 *
 * ************************************************************************* */
#ifndef _PIDREG_H_
#define _PIDREG_H_

class PidReg
{
    public:
        // Kp -  proportional gain
        // Ki -  Integral gain
        // Kd -  derivative gain
        // dt -  loop interval time
        // max - maximum value of manipulated variable
        // min - minimum value of manipulated variable
        PidReg( float dt, float max, float min, float Kp, float Kd, float Ki );

        // Returns the manipulated variable given a setpoint and current process value
        float PidReg_Calculate( float setpoint, float pv );
        void  PidSetParams( float dt, float max, float min, float Kp, float Kd, float Ki );

    private:
        float _dt;
        float _max;
        float _min;
        float _Kp;
        float _Kd;
        float _Ki;
        float _pre_error;
        float _integral;
};

#endif