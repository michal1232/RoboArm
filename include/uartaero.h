/** ************************************************************************
 * @file uartaero.h
 * @copyright RoboRuka
 * @copyright Rootic and Advanced Control
 * @copyright Dobrbovskeho 25
 * @copyright Brno, Czech Republic
 *
 * Created: 2020/21/10 14:30, Michal Veteska
 *
 * @addtogroup uartaero
 * @brief Comunication protokol for uart, standard areospace.
 *
 * ************************************************************************* */

#ifndef __UARTAERO_H__
#define __UARTAERO_H__

#include "Arduino.h"
#include "registers.h"

namespace UartAreo
{
    #define NODE_ID 10

    /* Makro pro výpočet velikosti datového typu */
    #define CANA_SIZE(type) type##_SIZE

    /* Makro pro inicializaci hlavicky ramce  */
    #define msg_header(msg, n_id, d_type, s_code, m_code)  \
                                msg.node_id      = n_id;   \
                                msg.data_type    = d_type; \
                                msg.service_code = s_code; \
                                msg.message_code = m_code;

    /* Macro hex BCD to dec */
    #define hexdec(x) ((x>'9') ? x-'A'+10 : x-'0')


    /*
    * Data type definitions.
    */

    #define  AS_NODATA   0     /* No data */
    #define  AS_ERROR    1     /* Emergency event */
    #define  AS_FLOAT    2     /* IEE754 single presision floating point */
    #define  AS_LONG     3     /* 2's complement 32 bit integer */
    #define  AS_ULONG    4     /* unsigned 32 bit integer */
    #define  AS_BLONG    5     /* bit coded 32 bit integer */
    #define  AS_SHORT    6
    #define  AS_USHORT   7
    #define  AS_BSHORT   8
    #define  AS_CHAR     9
    #define  AS_UCHAR    10
    #define  AS_BCHAR    11
    #define  AS_SHORT_2  12
    #define  AS_USHORT_2 13
    #define  AS_BSHORT_2 14
    #define  AS_CHAR_4   15
    #define  AS_UCHAR_4  16
    #define  AS_BCHAR_4  17
    #define  AS_CHAR_2   18
    #define  AS_UCHAR_2  19
    #define  AS_BCHAR_2  20
    #define  AS_MEMID    21
    #define  AS_CHKSUM   22
    #define  AS_ACHAR    23
    #define  AS_ACHAR_2  24
    #define  AS_ACHAR_4  25
    #define  AS_CHAR_3   26
    #define  AS_UCHAR_3  27
    #define  AS_BCHAR_3  28
    #define  AS_ACHAR_3  29
    #define  AS_DOUBLEH  30
    #define  AS_DOUBLEL  31

    /* Uživatelsky definované typy */
    #define  AS_TIME     100    /* Časová značka */
    #define  AS_FX_32_16 101    /* Fixed point */
    #define  AS_PASSWD   102    /* Hash hodnota hesla */


    /* Definice kodu servisu (polozka Service Code):       */

    #define IDS     0      /* Identification service              */ 
    #define NSS     1      /* Node Synchronization service        */
    #define DDS     2      /* Data Download Service               */
    #define DUS     3      /* Data Upload Service                 */
    #define SCS     4      /* Simulation Control Service          */
    #define TIS     5      /* Transmission Interval Service       */
    #define FPS     6      /* FLASH Programming Service           */
    #define STS     7      /* State Transmission Service          */
    #define FSS     8      /* Filter Setting Service              */
    #define TCS     9      /* Test Control Service                */
    #define BSS    10      /* Can Baudrate Setting Service        */
    #define NIS    11      /* Node-ID Setting Service             */
    #define MIS    12      /* Module Information Service          */
    #define MCS    13      /* Module Configuration Service        */
    #define CSS    14      /* CAN-ID Setting Service              */
    #define DSS    15      /* CAN-ID Distribution Setting Service */

    /* Uzivatelem definovane sluzby 100 - 255              */ 
    #define PASS    100    /* Read password code, set passowrd */
    #define RCRS    102    /* Read Configuration Register Service - page 0 */
    #define SCRS    103    /* Set Configuration Register Service  - page 0 */
    #define RCRS1   104    /* Read Configuration Register Service - page 1 */
    #define SCRS1   105    /* Set Configuration Register Service  - page 1 */
    #define RCRS2   106    /* Read Configuration Register Service - page 2 */
    #define SCRS2   107    /* Set Configuration Register Service  - page 2 */
    #define RCRS3   108    /* Read Configuration Register Service - page 3 */
    #define SCRS3   109    /* Set Configuration Register Service  - page 3 */
    #define RCRS4   110    /* Read Configuration Register Service - page 4 */
    #define SCRS4   111    /* Set Configuration Register Service  - page 4 */
    #define RCRS5   112    /* Read Configuration Register Service - page 5 */
    #define SCRS5   113    /* Set Configuration Register Service  - page 5 */
    #define RCRS6   114    /* Read Configuration Register Service - page 6 */
    #define SCRS6   115    /* Set Configuration Register Service  - page 6 */
    #define RCRS7   116    /* Read Configuration Register Service - page 7 */
    #define SCRS7   117    /* Set Configuration Register Service  - page 7 */
    #define RCRS8   118    /* Read Configuration Register Service - page 8 */
    #define SCRS8   119    /* Set Configuration Register Service  - page 8 */
    #define RCRS9   120    /* Read Configuration Register Service - page 9 */
    #define SCRS9   121    /* Set Configuration Register Service  - page 9 */
    #define RCRS10  122    /* Read Configuration Register Service - page 10 */
    #define SCRS10  123    /* Set Configuration Register Service  - page 10 */
    #define RCRS11  124    /* Read Configuration Register Service - page 11 */
    #define SCRS11  125    /* Set Configuration Register Service  - page 11 */
    #define RCRS12  126    /* Read Configuration Register Service - page 12 */
    #define SCRS12  127    /* Set Configuration Register Service  - page 12 */
    #define RCRS13  128    /* Read Configuration Register Service - page 13 */
    #define SCRS13  129    /* Set Configuration Register Service  - page 13 */
    #define RNS     130    /* Read Name Service */
    #define DDS_OK  131    /* Sluzba potvrzujici spravnost prenosu prez DDS pomoci checksumu */
    #define RCOK	 130
    #define SCOK	 131
    #define BCS     135    /* Command Bootloader Service */

    /* Definice návratových kódů */
    #define AS_INVALID -2
    #define AS_ABORT   -1
    #define AS_XOFF     0
    #define AS_XON      1
    #define AS_OK       0
    #define AS_END      2

    /* Definice zavaznosti chyb */
    #define EE_NOTICE   1
    #define EE_CAUTION  2
    #define EE_WARNING  3
    #define EE_FAULT    4

    /* Definice datove casti zpravy typu eed.              */

    typedef struct eed_data {
    uint16_t    error_code;
    uint8_t     operation_id;
    uint8_t     location_id;
    } EED_DATA;


    /* Definice datovych typu datove casti ramce can aero. */    
    typedef union msg_data {
    uint32_t   uint32;
    int32_t    int32;
    uint16_t   uint16;
    int16_t    int16;
    int16_t    int16_2[2];
    uint16_t   uint16_2[2];
    uint8_t    uint8;
    int8_t     int8;
    char     character;
    uint8_t    uint8_2[2];
    uint8_t    uint8_3[3];
    uint8_t    uint8_4[4];
    int8_t     int8_2[2];
    int8_t     int8_3[3];
    int8_t     int8_4[4];
    char     character_2[2];
    char     character_3[3];
    char     character_4[4];
    float     real32;
    EED_DATA eed;
    uint32_t   time;
    } MSG_DATA;

    /* Struktura definujici obsah ramce, ktery je prenasen po can. */
    /* Ramec je rozdeleny na hlavicku a datovou cast. Hlavicka je  */
    /* 4 bajty dlouha, datova cast je 0 - 4 bajty dlouha.          */

    typedef struct canaero_msg {
    uint8_t    node_id;       /* Adresa zarizeni na can */
    uint8_t    data_type;     /* Datovy typ prenasene hodnoty */
    uint8_t    service_code;  /* Kod servisu nebo 0 */
    uint8_t    message_code;  /* Inkrementuje se s kazdou zpravou */
    MSG_DATA message_data;  /* Prenasena hodnota - zavisle na typu hodnoty */
    } CANAERO_MSG;

    /* Datové typy pro ukazatele na funkce pro zpracování služeb protokolu */
    typedef int16_t (*AS_TX_FUNC)(uint16_t  id, uint8_t  len, uint8_t *frame);


    class UartAero
    {
    public:
        UartAero(AS_TX_FUNC ftx, String &rxString);

        void UartAeroRxProc();
        void UartAeroScrsReq(uint16_t u16_id, /*@unused@*/uint8_t u8_len, CANAERO_MSG *ps_frame);
        void UartAeroRcrsReq(uint16_t u16_id, /*@unused@*/uint8_t u8_len, CANAERO_MSG *ps_frame);
        int16_t UartAeroTxPacket(uint16_t  id, uint8_t  len, CANAERO_MSG *frame);
    private:
        /* Podle čísla spojení jsou z pole vybrány funkce pro zpracování přijmutých dat */

        AS_TX_FUNC  gptr_tx_func;
        String     *ptr_to_rx_str;

        /* globalni promenna xu16_node_id ma hodnotu kontanty, ale lze zmenit */
        uint16_t xu16_node_id = NODE_ID;

        void v_create_msg_frame(uint8_t *data_stream, CANAERO_MSG *msg);
        uint16_t u16_numcreat_short(uint8_t *data);
        uint32_t u32_numcreat_long(uint8_t *data);
        int16_t UartAeroDecodePacket(uint16_t  *id, uint8_t  *len, uint8_t *frame);
        uint8_t xu8_cana_sizeof(uint8_t data_type);

        void v_received_NSL(uint16_t id, uint8_t len, CANAERO_MSG *rec_frame);
        void numhexToString(String *ps_str, uint8_t data);

        uint8_t read_register(uint16_t addr, MSG_DATA *data);
        void xv_add_data_to_can(uint8_t data_type, uint16_t addr, MSG_DATA * data, uint8_t order);
        void set_register(uint16_t addr, uint8_t data_type, MSG_DATA * data);
        void xv_add_data_from_can(uint8_t data_type, uint16_t addr, MSG_DATA * data, uint8_t order);
    };  

}
#endif