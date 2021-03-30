/** ************************************************************************
 * @file uartaero.cpp
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

#include "uartaero.h"


namespace UartAreo
{
   /* Počet aktivních spojení */
   static int8_t gi8_connection_no = 0;

   uint8_t listPacket = 0;
   Registers::Registers reg;

   static const uint8_t gpu8_dechex[] = 
   {'0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  /* =============================================================
   * Definition of global functions or procedures that is adjustable also from other modules
   * ============================================================= */
   /**
    * @brief Construct a new Uart Aero:: Uart Aero object
    * 
    * @param frx 
    * @param ftx 
    * @param nod_proc 
    */
   UartAero::UartAero(AS_TX_FUNC ftx, String &rxString)
   {
      /* Překopírovat ukazatele na obslužné funkce */
      gptr_tx_func = ftx;
      ptr_to_rx_str = &rxString;
      gi8_connection_no++;
   }

   /**
    * @brief 
    * 
    */
   void UartAero::UartAeroRxProc()
   {
      uint16_t id;
      uint8_t  len;
      CANAERO_MSG  frame;
      uint8_t data_stream[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

      /* Pokusit se přijmout data */
      while (UartAeroDecodePacket(&id, &len, data_stream) == 0) {

         /* Vytvorit ramec CANAerospace */
         v_create_msg_frame(data_stream, &frame);
         
         /* Low Priority Node Service Data (NSL) */
         v_received_NSL(id, len, &frame);
      }
   }
   /** ************************************************************************
    * @brief Odpoved na pozadavek na nastaveni registru
    *
    * Funkce vytvori odpoved na pozadavek na nastaveni registru (sluzba SCRS).
    * Funkce akceptuje kody pro sluzby SCRS0 - SCRS3 a adresu registru premapovava
    * na zaklade kodu registru a kodu sluzby na adresovy rozsah 0 - 1023. Pro
    * nastaveni registru je volana externi funkce set_register_iface(). Po
    * nastaveni registru je vytvorena odpoved, ktera je vracena uzlu, ktery
    * pozadavek na nastaveni registru zaslal. Vyuzit je otevreny komunikacni kanal
    * (CANid+1).
    *
    * @param [in]  i8_connection  Handle instance komunikacniho protokolu
    * @param [in]  u16_id         CANid komunikacniho kanalu
    * @param [in]  u8_len         Delka prijmute zpravy
    * @param [in]  ps_frame       Ukazatel na prijmutou zpravu
    * @param [in]  xu16_node_id   Adresa aktulaniho uzlu
    *
    * @return n/a
    * ************************************************************************* */
   void UartAero::UartAeroScrsReq(uint16_t u16_id, /*@unused@*/uint8_t u8_len, CANAERO_MSG *ps_frame)
   {
      CANAERO_MSG response;  /* Odezva */
      uint8_t size;            /* Velikost zpravy */
      uint16_t addr;           /* Index cteneho parametru */

      if ((ps_frame->service_code >= SCRS) && (ps_frame->service_code <= SCRS9)) {

         addr = ps_frame->message_code + (((uint16_t)(ps_frame->service_code - SCRS))<<7);

         set_register(addr, ps_frame->data_type, &(ps_frame->message_data));

         response.data_type    = read_register(addr, &(response.message_data));
         response.node_id      = xu16_node_id;
         response.service_code = ps_frame->service_code;
         response.message_code = ps_frame->message_code;

         size = xu8_cana_sizeof(response.data_type);
         UartAeroTxPacket(u16_id+1, size, &response);
      }
   }

   /** ************************************************************************
    * @brief Odpoved na pozadavek na cteni registru
    *
    * Funkce vytvori odpoved na pozadavek na cteni registru (sluzba RCRS).
    * Funkce akceptuje kody pro sluzby RCRS0 - RCRS3 a adresu registru premapovava
    * na zaklade kodu registru a kodu sluzby na rozsah 0 - 1023. Pro
    * cteni registru je volana externi funkce read_register_iface(). Po
    * precteni obsahu registru je vytvorena odpoved, ktera je vracena uzlu, ktery
    * pozadavek na cteni resgistru zaslal. Vyuzit je otevreny komunikacni kanal
    * (CANId+1).
    *
    * @param [in]  i8_connection  Handle instance komunikacniho protokolu
    * @param [in]  u16_id         CANid komunikacniho kanalu
    * @param [in]  u8_len         Delka prijmute zpravy
    * @param [in]  ps_frame        Ukazatel na prijmutou zpravu
    * @param [in]  xu16_node_id   Adresa aktulaniho uzlu
    *
    * @return n/a
    * ************************************************************************* */
   void UartAero::UartAeroRcrsReq(uint16_t u16_id, /*@unused@*/uint8_t u8_len, CANAERO_MSG *ps_frame)
   {
      CANAERO_MSG response;  /* Odezva */
      uint16_t addr;           /* Index cteneho parametru */
      uint8_t size;            /* Velikost zpravy */

      //Serial.println("Obsluha RCRS");
      if ((ps_frame->service_code >= RCRS) && (ps_frame->service_code <= RCRS9)) {
         
         addr = ps_frame->message_code + (((uint16_t)(ps_frame->service_code - RCRS))<<7);

         response.data_type    = read_register(addr, &(response.message_data));
         response.node_id      = xu16_node_id;
         response.service_code = ps_frame->service_code;
         response.message_code = ps_frame->message_code;
         size = xu8_cana_sizeof(response.data_type);
         UartAeroTxPacket(u16_id+1, size, &response);
      }
   }

   /* =============================================================
   * Definition of local functions or procedures that is adjustable also from other modules
   * ============================================================= */
   /**
    * @brief 
    * 
    * @param data_stream 
    * @param msg 
    */
   void UartAero::v_create_msg_frame(uint8_t *data_stream, CANAERO_MSG *msg)
   {
      msg->node_id      = *(data_stream+0);
      msg->data_type    = *(data_stream+1);
      msg->service_code = *(data_stream+2);
      msg->message_code = *(data_stream+3);

      switch(msg->data_type) 
      {
         case AS_ERROR:
            msg->message_data.eed.error_code   = (uint16_t)u16_numcreat_short(data_stream+4);
            msg->message_data.eed.operation_id = *(data_stream+6);
            msg->message_data.eed.location_id  = *(data_stream+7);
            break;
         case AS_FLOAT:
            {
               uint32_t v;

               v = u32_numcreat_long(data_stream+4);
               msg->message_data.real32 = *( (float *)(&v));
            }
            break;
         case AS_LONG:
            msg->message_data.int32 = (uint32_t)u32_numcreat_long(data_stream+4);
            break;
         case AS_ULONG:
         case AS_BLONG:
         case AS_MEMID:
         case AS_CHKSUM:
         case AS_DOUBLEH:
         case AS_DOUBLEL:
         case AS_TIME:
         case AS_FX_32_16:
         case AS_PASSWD:
            msg->message_data.uint32 = (uint32_t)u32_numcreat_long(data_stream+4);
            break;
         case AS_SHORT:
            msg->message_data.int16 = (int16_t)u16_numcreat_short(data_stream+4);
            break;
         case AS_USHORT:
         case AS_BSHORT:
            msg->message_data.uint16 = (uint16_t)u16_numcreat_short(data_stream+4);
            break;
         case AS_SHORT_2:
            msg->message_data.int16_2[0] = (int16_t)u16_numcreat_short(data_stream+4);
            msg->message_data.int16_2[1] = (int16_t)u16_numcreat_short(data_stream+6);
            break;
         case AS_USHORT_2:
         case AS_BSHORT_2:
            msg->message_data.uint16_2[0] = (uint16_t)u16_numcreat_short(data_stream+4);
            msg->message_data.uint16_2[1] = (uint16_t)u16_numcreat_short(data_stream+6);
            break;
         case AS_CHAR:
            msg->message_data.int8 = (int8_t)(*(data_stream+4));
            break;
         case AS_UCHAR:
         case AS_BCHAR:
            msg->message_data.uint8 = (int8_t)(*(data_stream+4));
            break;
         case AS_ACHAR:
            msg->message_data.character = (char)(*(data_stream+4));
            break;
         case AS_CHAR_2:
            msg->message_data.int8_2[0] = (int8_t)(*(data_stream+4));
            msg->message_data.int8_2[1] = (int8_t)(*(data_stream+5));
            break;
         case AS_UCHAR_2:
         case AS_BCHAR_2:
         case AS_ACHAR_2:
            msg->message_data.uint8_2[0] = (uint8_t)(*(data_stream+4));
            msg->message_data.uint8_2[1] = (uint8_t)(*(data_stream+5));
            break;
         case AS_CHAR_3:
            msg->message_data.int8_3[0] = (int8_t)(*(data_stream+4));
            msg->message_data.int8_3[1] = (int8_t)(*(data_stream+5));
            msg->message_data.int8_3[2] = (int8_t)(*(data_stream+6));
            break;
         case AS_UCHAR_3:
         case AS_BCHAR_3:
         case AS_ACHAR_3:
            msg->message_data.uint8_3[0] = (uint8_t)(*(data_stream+4));
            msg->message_data.uint8_3[1] = (uint8_t)(*(data_stream+5));
            msg->message_data.uint8_3[2] = (uint8_t)(*(data_stream+6));
            break;
         case AS_CHAR_4:
            msg->message_data.int8_4[0] = (int8_t)(*(data_stream+4));
            msg->message_data.int8_4[1] = (int8_t)(*(data_stream+5));
            msg->message_data.int8_4[2] = (int8_t)(*(data_stream+6));
            msg->message_data.int8_4[3] = (int8_t)(*(data_stream+7));
            break;
         case AS_UCHAR_4:
         case AS_BCHAR_4:
         case AS_ACHAR_4:
            msg->message_data.uint8_4[0] = (uint8_t)(*(data_stream+4));
            msg->message_data.uint8_4[1] = (uint8_t)(*(data_stream+5));
            msg->message_data.uint8_4[2] = (uint8_t)(*(data_stream+6));
            msg->message_data.uint8_4[3] = (uint8_t)(*(data_stream+7));
            break;
         default:
            //Serial.println("Zadne data!");
            break;
      }
   }

   /**
    * @brief 
    * 
    * @param data_type 
    * @return uint8_t 
    */
   uint8_t UartAero::xu8_cana_sizeof(uint8_t data_type)
   {
      uint8_t size;

      switch(data_type) {
         case AS_NODATA:
            size = 4U;
            break;
         case AS_ERROR:
         case AS_FLOAT:
         case AS_LONG:
         case AS_ULONG:
         case AS_BLONG:
         case AS_SHORT_2:
         case AS_USHORT_2:
         case AS_BSHORT_2:
         case AS_CHAR_4:
         case AS_UCHAR_4:
         case AS_BCHAR_4:
         case AS_ACHAR_4:
         case AS_PASSWD:
            size = 8U;
            break;
         case AS_CHAR_3:
         case AS_UCHAR_3:
         case AS_BCHAR_3:
         case AS_ACHAR_3:
            size = 7U;
            break;
         case AS_SHORT:
         case AS_USHORT:
         case AS_BSHORT:
         case AS_CHAR_2:
         case AS_UCHAR_2:
         case AS_BCHAR_2:
         case AS_ACHAR_2:
            size = 6U;
            break;
         case AS_CHAR:
         case AS_UCHAR:
         case AS_BCHAR:
         case AS_ACHAR:
            size = 5U;
            break;
      }
      return(size);
   }

   /**
    * @brief 
    * 
    * @param data 
    * @return uint16_t 
    */
   uint16_t UartAero::u16_numcreat_short(uint8_t *data)
   {
      uint8_t i;
      uint16_t val = 0;

      for (i = 0; i<2; i++, data++) {
         val <<= 8;
         val += (uint16_t)(*data);
      }
      return(val);
   }

   /**
    * @brief 
    * 
    * @param data 
    * @return uint16_t 
    */
   uint32_t UartAero::u32_numcreat_long(uint8_t *data)
   {
      uint8_t i;
      uint32_t val = 0;

      for (i = 0; i<4; i++, data++) {
         val <<= 8;
         val += (uint32_t)(*data);
      }
      return(val);
   }


   int16_t UartAero::UartAeroDecodePacket(uint16_t  *id, uint8_t  *len, uint8_t *frame)
   {
      for(; listPacket <= 50; listPacket++)
      {
         if(ptr_to_rx_str->charAt(listPacket) == '0')
         {
            if(ptr_to_rx_str->charAt(listPacket+2) == 'F' && 
               ptr_to_rx_str->charAt(listPacket+3) == 'A' && 
               ptr_to_rx_str->charAt(listPacket+4) == '0' && 
               ptr_to_rx_str->charAt(listPacket+5) == '0')
            {
            *id = 2000U;
            *len = hexdec(ptr_to_rx_str->charAt(listPacket+1));
            uint8_t cnt = 0;
            for(uint8_t lenFrame = 0; lenFrame < (len[0]*2); lenFrame++)
            {
               if((listPacket+6+lenFrame) >= 50)
               {
                  listPacket = 0;
                  *ptr_to_rx_str = "";
                  return 1;
               }

               uint8_t v1, v2;
               v1 = hexdec(ptr_to_rx_str->charAt(listPacket+6+lenFrame));
               lenFrame++;
               v2 = hexdec(ptr_to_rx_str->charAt(listPacket+6+lenFrame));
               frame[cnt] = ((v1<<4) + v2);
               cnt++;
            }
            listPacket = listPacket+6+(len[0]*2);
            return 0;
            }
         }
      }

      listPacket = 0;
      *ptr_to_rx_str = "";
      return 1;
   }


   void UartAero::numhexToString(String *ps_str, uint8_t data)
   {
      *ps_str += (char)gpu8_dechex[data/16];
      *ps_str += (char)gpu8_dechex[data%16];
   }

   int16_t UartAero::UartAeroTxPacket(uint16_t  id, uint8_t  len, CANAERO_MSG *frame)
   {
      int32_t i32_tump;
      uint32_t ui32_tump;
      int16_t i16_tump;
      uint16_t u16_tump;
      int16_t i16_tump2;
      int16_t i16_tump1;
      uint16_t u16_tump2;
      uint16_t u16_tump1;

      String tx_str = "";
      tx_str.reserve(50);

      numhexToString(&tx_str, len);
      numhexToString(&tx_str, ((id*32)>>8U)&0xFFU);
      numhexToString(&tx_str, ((id*32)&0xFFU));
      numhexToString(&tx_str, frame->node_id);
      numhexToString(&tx_str, frame->data_type);
      numhexToString(&tx_str, frame->service_code);
      numhexToString(&tx_str, frame->message_code);

      switch(frame->data_type) 
      {
         case AS_FLOAT:    
            ui32_tump = *(uint32_t *)&(frame->message_data.real32);
            numhexToString(&tx_str, (ui32_tump>>24)&0xFF);
            numhexToString(&tx_str, (ui32_tump>>16)&0xFF);
            numhexToString(&tx_str, (ui32_tump>>8)&0xFF);
            numhexToString(&tx_str, (ui32_tump&0xFF));  
            break;
         case AS_LONG:
            i32_tump = frame->message_data.int32;
            numhexToString(&tx_str, ((i32_tump>>24)&0xFF));
            numhexToString(&tx_str, ((i32_tump>>16)&0xFF));
            numhexToString(&tx_str, ((i32_tump>>8)&0xFF));
            numhexToString(&tx_str, (i32_tump&0xFF));    
            break;
         case AS_ULONG:
         case AS_BLONG:
         case AS_MEMID:
         case AS_CHKSUM:
         case AS_DOUBLEH:
         case AS_DOUBLEL:
         case AS_TIME:
         case AS_FX_32_16:
         case AS_PASSWD:
            ui32_tump = frame->message_data.uint32; 
            numhexToString(&tx_str, ((ui32_tump>>24)&0xFF));
            numhexToString(&tx_str, ((ui32_tump>>16)&0xFF));
            numhexToString(&tx_str, ((ui32_tump>>8)&0xFF));
            numhexToString(&tx_str, (ui32_tump&0xFF));
            break;
         case AS_SHORT:
            i16_tump = frame->message_data.int16;
            numhexToString(&tx_str, ((i16_tump>>8)&0xFF));
            numhexToString(&tx_str, (i16_tump&0xFF));
            break;
         case AS_USHORT:
         case AS_BSHORT:
            u16_tump = frame->message_data.uint16;
            numhexToString(&tx_str, ((u16_tump>>8)&0xFF));
            numhexToString(&tx_str, (u16_tump&0xFF));
            break;
         case AS_SHORT_2:
            i16_tump2 = frame->message_data.int16_2[0];
            i16_tump1 = frame->message_data.int16_2[1]; 
            numhexToString(&tx_str,((i16_tump1>>8)&0xFF));
            numhexToString(&tx_str,(i16_tump1&0xFF));
            numhexToString(&tx_str,((i16_tump2>>8)&0xFF));
            numhexToString(&tx_str,((i16_tump2)&0xFF));
            break;
         case AS_USHORT_2:
         case AS_BSHORT_2:
            u16_tump2 = frame->message_data.uint16_2[0];
            u16_tump1 = frame->message_data.uint16_2[1];
            numhexToString(&tx_str,((u16_tump1>>8)&0xFF));
            numhexToString(&tx_str,(u16_tump1&0xFF));
            numhexToString(&tx_str,((u16_tump2>>8)&0xFF));
            numhexToString(&tx_str,((u16_tump2)&0xFF));
            break;
         case AS_CHAR:
            numhexToString(&tx_str,frame->message_data.int8);
            break;
         case AS_UCHAR:
         case AS_BCHAR:
            numhexToString(&tx_str,frame->message_data.uint8);
            break;
         case AS_ACHAR:
            numhexToString(&tx_str,frame->message_data.character);
            break;
         case AS_CHAR_2:
            numhexToString(&tx_str,frame->message_data.int8_2[0]);
            numhexToString(&tx_str,frame->message_data.int8_2[1]);
            break;
         case AS_UCHAR_2:
         case AS_BCHAR_2:
         case AS_ACHAR_2:
            numhexToString(&tx_str,frame->message_data.uint8_2[0]);
            numhexToString(&tx_str,frame->message_data.uint8_2[0]);
            break;
         case AS_CHAR_3:
            numhexToString(&tx_str,frame->message_data.int8_3[0]);
            numhexToString(&tx_str,frame->message_data.int8_3[1]);
            numhexToString(&tx_str,frame->message_data.int8_3[2]);
            break;
         case AS_UCHAR_3:
         case AS_BCHAR_3:
         case AS_ACHAR_3:
            numhexToString(&tx_str,frame->message_data.uint8_3[0]);
            numhexToString(&tx_str,frame->message_data.uint8_3[1]);
            numhexToString(&tx_str,frame->message_data.uint8_3[2]);
            break;
         case AS_CHAR_4:
            numhexToString(&tx_str,frame->message_data.int8_4[0]);
            numhexToString(&tx_str,frame->message_data.int8_4[1]);
            numhexToString(&tx_str,frame->message_data.int8_4[2]);
            numhexToString(&tx_str,frame->message_data.int8_4[3]);
            break;
         case AS_UCHAR_4:
         case AS_BCHAR_4:
         case AS_ACHAR_4:
            numhexToString(&tx_str,frame->message_data.uint8_4[0]);
            numhexToString(&tx_str,frame->message_data.uint8_4[1]);
            numhexToString(&tx_str,frame->message_data.uint8_4[2]);
            numhexToString(&tx_str,frame->message_data.uint8_4[3]);
            break;
         default:
            //Serial.println("Zadne data!");
            break;
      }
      tx_str += (char)13;
      Serial.print(tx_str);
   }

   /* ****************************************************************
   * Popis:
   * **************************************************************** */
   void UartAero::v_received_NSL(uint16_t id, uint8_t len, CANAERO_MSG *rec_frame)
   {
      //Serial.println("Spracovani NSL.");
      if ((id & 0x01) == 0) {
         switch (rec_frame->service_code) {
            case RCRS:/* Read configuration register service */
            case RCRS1:
            case RCRS2:
            case RCRS3:
            case RCRS4:
            case RCRS5:
            case RCRS6:
            case RCRS7:
            case RCRS8:
               UartAeroRcrsReq(id, len, rec_frame);
               break;
            case SCRS:/* Set configuration register service */
            case SCRS1:
            case SCRS2:
            case SCRS3:
            case SCRS4:
            case SCRS5:
            case SCRS6:
            case SCRS7:
            case SCRS8:
               UartAeroScrsReq(id, len, rec_frame);
               break;
         }
      }
   }

   /*
   * @brief Read from database of register
   * The function is to read the Module Configuration Registry.
   *
   * @param [in] addr			can register address
   * @param [in] *data		Data in can msg format
   *
   * @return data_type		return data type of send message
   */
   uint8_t UartAero::read_register(uint16_t addr, MSG_DATA *data)
   {
      uint8_t data_type = AS_NODATA;
      uint16_t screach_addr = 0;
      uint32_t size_reg = 0;
      uint32_t cnt_reg = 0;
      int16_t cnt = -1;
      uint32_t size;
      size = reg.regGetSizeDatabase();

      for(uint16_t i = 0; i < size; i++)
      {  
         size_reg = reg.regGetRegSize(i);
         cnt += size_reg;

         if((uint16_t)cnt == addr)
         {
            screach_addr = i;
            cnt_reg = size_reg-1;
            break;
         }
         else if((uint16_t)cnt > addr)
         {
            screach_addr = i;
            cnt_reg = addr - (cnt - size_reg) - 1;
            break;
         }
      }
         /*Set data type*/
         data_type = reg.regGetDataType(screach_addr);
         /*Add data to can msg. Order in data are pick by data type*/
         xv_add_data_to_can(data_type, screach_addr, data, cnt_reg);

         return (data_type);
 
   }

   /*
   * @brief Set value to can msg
   *
   * This function set value to can msg. The decision to which the reports will be assigned depends on the data type of message.
   *
   * @param [in] type 		structure type
   * @param [in] data_type	data type of send message
   * @param [in] addr			address of pointer in database
   * @param [in] order		order in array
   * @param [out] data		send data in can msg format
   */
   void UartAero::xv_add_data_to_can(uint8_t data_type, uint16_t addr, MSG_DATA * data, uint8_t order)
   {
      switch(data_type)
      {
         float *fp32_point_to_can;
         int32_t *i32_point_to_can;
         uint32_t *u32_point_to_can;
         uint32_t *b32_point_to_can;
         int16_t *i16_point_to_can;
         uint16_t *u16_point_to_can;
         uint16_t *b16_point_to_can;
         int8_t *i8_point_to_can;
         uint8_t *u8_point_to_can;

         case AS_FLOAT:  
            fp32_point_to_can = (float *)reg.regGetPointer(addr); 
            data->real32 = fp32_point_to_can[order];
            break;
         case AS_LONG:
            i32_point_to_can = (int32_t *)reg.regGetPointer(addr); 
            data->int32 = i32_point_to_can[order];
            break;
         case AS_ULONG:
            u32_point_to_can = (uint32_t *)reg.regGetPointer(addr); 
            data->uint32 = u32_point_to_can[order];
            break;
         case AS_BLONG:
            b32_point_to_can = (uint32_t *)reg.regGetPointer(addr); 
            data->uint32 = b32_point_to_can[order];
            break;
         case AS_SHORT:
            i16_point_to_can = (int16_t *)reg.regGetPointer(addr); 
            data->int16 = i16_point_to_can[order];
            break;
         case AS_USHORT:
            u16_point_to_can = (uint16_t *)reg.regGetPointer(addr); 
            data->uint16 = u16_point_to_can[order];
            break;
         case AS_BSHORT:
            b16_point_to_can = (uint16_t *)reg.regGetPointer(addr); 
            data->uint16 = b16_point_to_can[order];
            break;
         case AS_CHAR:
            i8_point_to_can = (int8_t *)reg.regGetPointer(addr); 
            data->int8 = i8_point_to_can[order];
            break;
         case AS_UCHAR:
            u8_point_to_can = (uint8_t *)reg.regGetPointer(addr); 
            data->uint8 = u8_point_to_can[order];
            break;
         case AS_BCHAR:
            u8_point_to_can = (uint8_t *)reg.regGetPointer(addr); 
            data->uint8 = u8_point_to_can[order];
            break;
         default:
            break;
      }
   }

   /*
   * @brief write from database of register
   *
   * The function is to write the Module Configuration Registry.
   * After this function data in databese are update.
   *
   * @param [in] addr			can register address
   * @param [in] *data		Data in can msg format
   * @param [in] data_type	data type of arrive message
   */
   void UartAero::set_register(uint16_t addr, uint8_t data_type, MSG_DATA * data)
   {
      uint16_t screach_addr = 0;
      uint32_t size_reg = 0;
      uint32_t cnt_reg = 0;
      int16_t cnt = -1;
      uint32_t size;
      size = reg.regGetSizeDatabase();

      //Serial.print("size"); Serial.println(size);

      for(uint16_t i = 0; i < size; i++)
      {  
         size_reg = reg.regGetRegSize(i);
         cnt += size_reg;
         //Serial.print("size_reg"); Serial.println(size_reg);
         //Serial.print("cnt"); Serial.println(cnt);
         if((uint16_t)cnt == addr)
         {
            screach_addr = i;
            cnt_reg = size_reg-1;
            //Serial.print("screach_addr"); Serial.println(screach_addr);
            //Serial.print("cnt_reg"); Serial.println(cnt_reg);
            break;
         }
         else if((uint16_t)cnt > addr)
         {
            screach_addr = i;
            cnt_reg = addr - (cnt - size_reg) - 1;
            //Serial.print("screach_addr"); Serial.println(screach_addr);
            //Serial.print("cnt_reg"); Serial.println(cnt_reg);
            break;
         }
      }

	   /*Add data to can msg. Order in data are pick by data type*/
	   xv_add_data_from_can(data_type, screach_addr, data, cnt_reg);
   }

/*
 * @brief Set value from can msg
 *
 * This function set value from can msg. The decision to which the reports will be assigned depends on the data type of message.
 *
 * @param [in] type 		structure type
 * @param [in] data_type	data type of send message
 * @param [in] addr			address of pointer in database
 * @param [in] order		order in array
 * @param [in] data			send data in can msg format
 * @param [out]	point		save data in database
 */
void UartAero::xv_add_data_from_can(uint8_t data_type, uint16_t addr, MSG_DATA * data, uint8_t order)
{
   /* Zpracovani externich registru */
	   switch(data_type)
	   {
	   	   float *fp32_point_from_can;
	   	   int32_t *i32_point_from_can;
	   	   uint32_t *u32_point_from_can;
	   	   uint32_t *b32_point_from_can;
	   	   int16_t *i16_point_from_can;
	   	   uint16_t *u16_point_from_can;
	   	   uint16_t *b16_point_from_can;
	   	   int8_t *i8_point_from_can;
	   	   uint8_t *u8_point_from_can;

			case AS_FLOAT:
			   fp32_point_from_can = (float *)reg.regGetPointer(addr); 
			   fp32_point_from_can[order] = data->real32;
			   break;
			case AS_LONG:
				i32_point_from_can = (int32_t *)reg.regGetPointer(addr); 
				i32_point_from_can[order] = data->int32;
				break;
			case AS_ULONG:
				u32_point_from_can = (uint32_t *)reg.regGetPointer(addr); 
				u32_point_from_can[order] = data->uint32;
				break;
			case AS_BLONG:
				b32_point_from_can = (uint32_t *)reg.regGetPointer(addr); 
				b32_point_from_can[order] = data->uint32;
				break;
			case AS_SHORT:
				i16_point_from_can = (int16_t *)reg.regGetPointer(addr); 
				i16_point_from_can[order] = data->int16;
				break;
			case AS_USHORT:
				u16_point_from_can = (uint16_t *)reg.regGetPointer(addr); 
				u16_point_from_can[order] = data->uint16;
				break;
			case AS_BSHORT:
				b16_point_from_can = (uint16_t *)reg.regGetPointer(addr); 
				b16_point_from_can[order] = data->uint16;
				break;
			case AS_CHAR:
				i8_point_from_can = (int8_t *)reg.regGetPointer(addr); 
				i8_point_from_can[order] = data->int8;
				break;
			case AS_UCHAR:
			case AS_BCHAR:
				u8_point_from_can = (uint8_t *)reg.regGetPointer(addr); 
				u8_point_from_can[order] = data->uint8;
				break;
			default:
				break;
	   }
   }
}



