#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "lib_NDEF_URI.h"
#include "lib_NDEF_SMS.h"
#include "lib_NDEF_Text.h"
#include "lib_NDEF_Email.h"
#include "lib_NDEF_Geo.h"
#include "lib_95HFConfigManager.h"
#include "miscellaneous.h"
#include "lib_95HFConfigManager.h"
#include "lib_wrapper.h"
#include "lib_NDEF_URI.h"
#include "drv_spi.h"

#define SerialPort Serial
#define BULK_MAX_PACKET_SIZE            0x00000040
#define PICC_TYPEA_ACConfigA            0x27  /* backscaterring */
#define PICC_TYPEB_ARConfigD            0x0E  /* card demodulation gain */
#define PICC_TYPEB_ACConfigA            0x17  /* backscaterring */
#define PICC_TYPEF_ACConfigA            0x17  /* backscaterring */
uint8_t TT1Tag[NFCT1_MAX_TAGMEMORY];
uint8_t TT2Tag[NFCT2_MAX_TAGMEMORY];
uint8_t TT3Tag[NFCT3_MAX_TAGMEMORY];
uint8_t *TT3AttribInfo = TT3Tag, *TT3NDEFfile = &TT3Tag[NFCT3_ATTRIB_INFO_SIZE];
uint8_t CardCCfile      [NFCT4_MAX_CCMEMORY];
uint8_t CardNDEFfileT4A [NFCT4_MAX_NDEFMEMORY];
uint8_t CardNDEFfileT4B [NFCT4_MAX_NDEFMEMORY];
uint8_t TT5Tag[NFCT5_MAX_TAGMEMORY];
sURI_Info url; 
extern uint8_t NDEF_Buffer []; 
extern DeviceMode_t devicemode;
sRecordInfo_uri RecordStruct;
int8_t TagType = TRACK_NOTHING;
bool TagDetected = false;
bool terminal_msg_flag = false ;
uint8_t status = ERRORCODE_GENERIC;
static char dataOut[256];

#define X_NUCLEO_NFC03A1_LED1 D7
#define X_NUCLEO_NFC03A1_LED2 D6
#define X_NUCLEO_NFC03A1_LED3 D5
#define X_NUCLEO_NFC03A1_LED4 D4

void setup() {

  ConfigManager_HWInit();  
  terminal_msg_flag = true;
  digitalWrite(X_NUCLEO_NFC03A1_LED1, HIGH);
}


/* Loop ----------------------------------------------------------------------*/



void loop()
{
  devicemode = PCD;
  TagType = ConfigManager_TagHunting(TRACK_ALL);
  TagDetected = TagType==TRACK_NFCTYPE2;

  if (TagDetected == true)
  {       
    TagDetected = false;
      
    status = ERRORCODE_GENERIC;
    SerialPort.println( "Tag detecté ! Tentative de lecture :");
    
    delay(500); // <-------------------


    status = PCDNFCT2_ReadNDEF();
                
    if ( status == RESULTOK )
    {
      Serial.println("Lecture OK");
               
        status = NDEF_IdentifyNDEF( &RecordStruct, NDEF_Buffer);
          
        if(status == RESULTOK && RecordStruct.TypeLength != 0)
        {
          if (NDEF_ReadURI(&RecordStruct, &url)==RESULTOK) /*---if URI read passed---*/
          {
            char* dataNFC = (char *)url.URI_Message;
            char areneId[5];
            char nomEpreuve[50];
            char inscruction[200];
  
            char * pch;
            pch = strtok (dataNFC,":");
            while (pch != NULL)
            {
              printf ("%s\n",pch);
              pch = strtok (NULL, ":");
            }

          }
        
        }
      }
  }
}

