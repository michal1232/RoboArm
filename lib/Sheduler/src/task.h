/** *************************************************************** 
 * @file
 * @copyright UNIS a.s.
 * @copyright Aerospace and Advanced Control
 * @copyright Jundrovska 33
 * @copyright Brno, Czech Republic
 *
 * Created: 2015/05/26 09:17, user svacina
 *
 * $HeadURL: $
 * $Revision: $
 * $Date: $
 * $Author: svacina $
 *
 * @addtogroup  task
 * @{
 * **************************************************************** */
#ifndef __TASK_H__
#define __TASK_H__

#include "Arduino.h"

namespace taskNS {

    struct task {
        void (*exe)(void);  /* Co se ma provadet */
        uint16_t u16_time;    /* Perioda provadeni - defaultni hodnota */
        uint16_t u16_cnt;     /* Pracovni citac */
        bool    b_active;    /* Pro spusteni je uloha defaultne povolena/zakazana*/
    };

    class taskCls
    {
    public:

        void   xv_task_ini(struct task **rec_task_list, uint8_t u8_size);
        void   xv_task_chk(void);
        void   xv_task_chk_one_pass(void);
        void   xv_task_timeinc(void);
        uint32_t xu32_task_glob_time(void);
        void   xv_task_on(struct task *t);
        void   xv_task_off(struct task *t);
        bool   xb_task_is_on(struct task *t);
        void   xv_task_set_period(struct task *t, uint16_t u16_period);
        uint16_t xu16_task_queue_size(void);

    private:

        #ifndef TASK_QUEUE_SIZE
        #define TASK_QUEUE_SIZE 25
        #endif

        void v_task_queue_update(uint8_t u8_task_idx);
    };
}

#endif   /* #ifndef __TASK_H__ */

/** @} */

