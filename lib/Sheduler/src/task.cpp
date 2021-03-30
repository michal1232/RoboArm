/** ************************************************************************
 * @file
 * @copyright UNIS a.s.
 * @copyright Aerospace and Advanced Control
 * @copyright Jundrovska 33
 * @copyright Brno, Czech Republic
 *
 * Created: 2015/05/05 16:47, user svacina
 *
 * $HeadURL: $
 * $Revision: $
 * $Date: $
 * $Author: svacina $
 *
 * @addtogroup  task
 * @{
 *
 * @brief Funkce pro rizeni chodu programu
 *
 * Modul obsahuje primitivni task scheduler, ktery spousti ulohy
 * v predem nastavenem case. Kazda uloha je popsana strukturou task,
 * ktera obsahuje ukazatel na funkci, ktera se ma vykonat, pracovni pocitadlo
 * (casovac) a hodnotu periody, po ktere se ma funkce vykonat. Rovnez
 * obsahuje atribut, ktery urcuje, zda je uloha aktivni nebo ne.
 * K tomu, aby bylo mozne modul vyuzivat musi byt zajisteno pravidelne
 * volani funkce x_task_timeinc() z preruseni casovace
 * ************************************************************************* */
#define __TASK_INTERNAL__
#include "task.h"

namespace taskNS
{
    static struct task  **grec_task_list;           /**< Ukazatel na seznam definovanych uloh */
    static uint8_t  gu8_task_size;                    /**< Pocet uloh v seznamu */
    static uint8_t  gu8_task_queue[TASK_QUEUE_SIZE];  /**< Pole pro ulozeni fronty uloh */
    static uint8_t  gu8_task_queue_start;             /**< Index prvni ulohy ve fronte */
    static uint8_t  gu8_task_queue_end;               /**< Index posledni ulohy ve fronte + 1 */
    static uint32_t gu32_glob_time;                   /**< Pocitadlo casu */


    /** ************************************************************************
     * @brief Vrati aktualni velikost fronty uloh
     *
     * Ulohy se ukladaji do pole gu8_task_queue[]. Pocatek fronty je dan indexem
     * gu8_task_queue_start, konec potom gu8_task_queue_end. Na zaklade techto
     * indexu je vypocten pocet uloh, ktery se ve fronte nachazi.
     *
     * @param [in] gu8_task_queue_end    Index prvni ulohy ve fronte
     * @parma [in] gu8_task_queue_start  Index posledni ulohy ve fronte + 1
     *
     * @return Pocet uloh ve fronte
     * ************************************************************************* */
    uint16_t taskCls::xu16_task_queue_size(void)
    {
        uint16_t u16_size;

        if (gu8_task_queue_end >= gu8_task_queue_start) {
            u16_size = gu8_task_queue_end - gu8_task_queue_start;
        } else {
            u16_size = TASK_QUEUE_SIZE - gu8_task_queue_start + gu8_task_queue_end;
        }

        return(u16_size);
    }

    /** ************************************************************************
     * @brief Aktivace ulohy
     *
     * Pokud se atribut b_active, ktery je prirazeny aktualni uloze, nachazi v
     * logickem stavu FALSE, je uloha neaktivni. To znamena, ze neni nikdy
     * spustena. Jakmile se polozka nastavi na hodnotu TRUE, stane se uloha aktivni
     * a je periodicky spoustena s periodou, ktera je nastavena atributem u16_time.
     * Pri zmene stavu ulohy z neaktivni do aktivni je nastaveno pocitadlo u16_cnt
     * na hodnot u16_time. To znamena, ze k prvnimu vyvolani funkce nedojde
     * okamzite, ale az po case u16_time od aktivace ulohy.
     *
     * @param [in]  t           Ukazatel na ulohu, ktera ma byt aktivovana
     * @param [in]  t->u16_time Perioda pro spousteni ulohy
     * @param [out] t->u16_cnt  Pracovni pocitadlo pro urceni casu spusteni
     * @param [out] t->b_active Urcuje, ze je uloha aktivni
     *
     * @return n/a
     * ************************************************************************* */
    void taskCls::xv_task_on(struct task *t)
    {
        if (t != NULL) {
            t->u16_cnt = t->u16_time;  /* Nainicializovat */
            t->b_active = true;        /* Ulohu aktivovat */
        }
    }

    /** ************************************************************************
     * @brief Deaktivace ulohy
     *
     * Pokud je atribut b_active, ktery je prirazeny aktualni uloze, nastaven
     * do logickeho stasvu TRUE, je uloha periodicky spoustena v zavislosti
     * na nastaveni polozky u16_time struktury task aktualni ulohy. Volanim
     * funkce se polozka b_active nastavi na logickou hodnotu FALSE, coz
     * ma za dusledek, ze uloha po zavolani teto funkce jiz nebude vyvolana.
     * Opetovneho spusteni ulohy lze dosahnout az volanim funkce task_on().
     * Interval pro spusteni ulohy se vsak zacne merit v okamziku zavolani
     * funkce task_on() opet od nuly.
     *
     * @param [in]  t             Ukazatel na ulohu, ktera ma byt deaktivovana
     * @param [out] t->b_active   Urcuje, ze je uloha neaktivni
     *
     * @return n/a
     * ************************************************************************* */
    void taskCls::xv_task_off(struct task *t)
    {
        if (t != nullptr) { 

            t->b_active = false; /* Ulohu deaktivovat */
        }
    }


    /** ************************************************************************
     * @brief Vrati informaci o aktivite ulohy
     *
     * Vrati stav, ve kterem se uloha nachazi, t.j. zda je aktivni nebo nekativni.
     * Fyzicky je vracen obsah polozky b_active strktury task ulohy.
     *
     * @param [in] t           Ukazatel na ulohu, u niz ma byt zjisten stav
     * @param [in] t->b_active Informace o aktivite ulohy
     *
     * @retval TRUE  Uloha je aktivni
     * @retval FALSE Uloha je neaktivni
     * ************************************************************************* */
    bool taskCls::xb_task_is_on(struct task *t)
    {
        bool res = false;

        if (t != nullptr) {
            res = t->b_active;
        }

        return(res);
    }

    /** ************************************************************************
     * @brief Nastavi periodu pro volani tasku
     * 
     * Nastavi periodu vyvolavani dane funkce. Funkce je dana svym indexem v
     * seznamu a cas poctem casovych jednotek. Pokud dojde ke zmene periody za
     * provozu, neni zkracena perioda prave probihajici.
     *
     * @param [in]  t           Ukzatel na ulohy, u ktere se ma nastavit perioda
     * @param [in]  u16_period  Perioda, ktera se ma na stavit
     * @param [out] t->u16_time Perioda se kterou se ma task volat
     *
     * @return n/a
     * ************************************************************************* */
    void taskCls::xv_task_set_period(struct task *t, uint16_t u16_period)
    {
        if (t != NULL)
        { 
            t->u16_time = u16_period;
        }   
    }

    /** ************************************************************************
     * @brief Inicializace internich datovych struktur
     *
     * Funkce inicializuje seznam uloh, ktere modul obhospodaruje a uvede
     * interni datove promenne do vychoziho stavu.
     *
     * @param [in]  rec_task_list        Ukazatel na pole uloh, ktere se maji provadet
     * @param [in]  u8_size              Pocet uloh v seznamu
     * @param [out] gu8_task_size        Pocet uloh v seznamu
     * @param [out] grec_task_list       Ukazatel na pole uloh, ktere se maji provadet
     * @param [out] gu8_task_queue_start Index prvni ulohy ve fronte uloh
     * @param [out] gu8_task_queue_end   Index posledni ulohy ve fronte uloh + 1
     *
     * @retval
     * ************************************************************************* */
    void taskCls::xv_task_ini(struct task **rec_task_list, uint8_t u8_size)
    {
        gu8_task_size  = u8_size;
        grec_task_list = rec_task_list;
        gu8_task_queue_start = 0U;
        gu8_task_queue_end   = 0U;
    }


    /** ************************************************************************
     * @brief Periodicka kontrola uloh a spousteni uloh ve fronte
     *
     * Funkce je nekonecna smycka, ktera kontroluje frontu uloh. Pokud funkce
     * najde ve fronte ulohu, spusti ji, pocka na ukonceni spustene funkce a
     * posune index prvni ulohy gu8_tas_queue_start na dalsi prvek. Tak pokracuje
     * dale, dokud se zasobnik uloh nevyprazdni.
     *
     * Poznamka:
     *    Aktivni makro __DEBUG_TASK_PROFILE__ vklada do funkce kod,
     *    ktery umoznuje mereni spotreby casu spoustene funkce. Mereni
     *    casu je realizovano volanim dvojice funkci xv_profile_start()
     *    a xv_profile_stop().
     *
     * @param [in,out] gu8_task_queue_start   Index 1. ulohy ve fronte uloh
     * @param [in]     gu8_task_queue_end     Index posledni ulohy ve fronte + 1
     * @param [in]     gu8_task_queue         Fronta uloh
     *
     * @return n/a
     * ************************************************************************* */
    void taskCls::xv_task_chk(void)
    {
        while(true) {
            taskCls::xv_task_chk_one_pass();
        }
    }

    void taskCls::xv_task_chk_one_pass(void)
    {
        uint8_t task_idx;

        while ( gu8_task_queue_start != gu8_task_queue_end) {

            task_idx = gu8_task_queue[gu8_task_queue_start];
                
            /* Tato podminka osetruje pripad, kdy je task ve fronte, ale v mezicase
            * doslo k jeho vypnuti
            */
            if (grec_task_list[task_idx]->b_active == true) {

        #ifdef __DEBUG_TASK_PROFILE__
            xv_profile_start(task_idx);
        #endif
            (*grec_task_list[task_idx]->exe)();

        #ifdef __DEBUG_TASK_PROFILE__
            xv_profile_stop(task_idx);
        #endif
            }

            gu8_task_queue_start++;

            if (gu8_task_queue_start >= TASK_QUEUE_SIZE) {
            gu8_task_queue_start = 0U;
            }
        }
    }


    /** ************************************************************************
     * @brief Update pocitadel uloh v seznamu
     *
     * Funkce je volana z preruseni od casovace. Fukce prochazi seznam uloh a
     * u kazde aktivni ulohy dekrementuje pocitadlo, ktere urcuje, kdy se ma
     * uloha vykonat. Pokud je hodnota pocitadla nulova, prida se uloha do
     * fronty uloh a pocitadlo se nainicializuje na hodnotu polozky u16_time.
     * K dalsimu zarazeni do fronty dojde po opetovnem vynulovani polozky
     * u16_cnt. To se opakuje, dokud je uloha aktivni.
     *
     * Funkce soucasne inkrementuje pocitadlo globalniho casu.
     *
     * @param [in,out] gu32_glob_time    Pocitadlo globalniho csu
     * @param [in]     gu8_task_size     Pocet uloh v seznamu
     * @param [in]     grec_task_list    Ukazatel na pole seznamu uloh
     *
     * @retval
     * ************************************************************************* */
    void taskCls::xv_task_timeinc(void)
    {
        uint8_t u8_task_idx;

        gu32_glob_time++;

        for (u8_task_idx = 0U; u8_task_idx < gu8_task_size; u8_task_idx++) {

            if (grec_task_list[u8_task_idx]->b_active != false) {

                /* Decrement task counter */
                if (grec_task_list[u8_task_idx]->u16_cnt != 0U) {
                    grec_task_list[u8_task_idx]->u16_cnt--;
                }

                /* Add task into queue */
                if (grec_task_list[u8_task_idx]->u16_cnt == 0U) {
                    taskCls::v_task_queue_update(u8_task_idx);

                    /* Reinitialization of task counter */
                    grec_task_list[u8_task_idx]->u16_cnt = grec_task_list[u8_task_idx]->u16_time;
                }
            }
        }
    }


    /** ************************************************************************
     * @brief Provede update fronty uloh
     *
     * Funkce prida ulohu, ktera je dana parametrem u8_task_idx do fronty uloh,
     * ktere se maji vykonat. Fronta uloh je implementovana kruhovym bufferem
     * gu8_task_queue. Index ulohy je do pole ulozen na pozici danou indexem
     * gu8_task_queue_end. Nasledne je index zvetsen o jedna, cimz ukazuje na
     * dalsi volnou pozici.
     *
     * @param [in]     u8_task_idx        Index ulohy, ktera se ma zaradi do fronty
     * @param [out]    gu8_task_queue     Pole obsahujici frontu uloh
     * @param [in,out] gu8_task_queue_end Index posledni ulohy ve fronte + 1
     *
     * @return n/a
     * ************************************************************************* */
    void taskCls::v_task_queue_update(uint8_t u8_task_idx)
    {
        uint8_t u8_idx;

        /* Vypocitat si dalsi index ve fronte uloh */
        u8_idx = gu8_task_queue_end + 1;
        if (u8_idx >= TASK_QUEUE_SIZE) {
            u8_idx = 0U;
        }

        /* Pokud neni fronta uloh plna, pridat dalsi ulohu */
        if (u8_idx != gu8_task_queue_start) {

            gu8_task_queue[gu8_task_queue_end] = u8_task_idx;
            gu8_task_queue_end                 = u8_idx;
        }
    }

    /** ************************************************************************
     * @brief Vrati aktualni hodnot pocitadla casu
     *
     * Po pripojeni k napajeni je nastaveno pocitadlo casu gu32_glob_time na nulu,
     * s kazdym volanim funkce x_task_timeinc() se hodnota pocitadla zvedne
     * o jednicku. Pocitadlo tak reprezentuje cas, ktery ubehl od pripojeni
     * zarizeni k napajeni v jednotkach, ktere jsou zavisle na frekvenci
     * volani funkce x_task_timeinc().
     *
     * @param [in] gu32_glob_time    Aktualni hodnota pocitadla casu
     *
     * @return Aktualni hodnota pocitadla casu
     * ************************************************************************* */
    uint32_t taskCls::xu32_task_glob_time(void)
    {
        return(gu32_glob_time);
    }


    /** @} */
}



