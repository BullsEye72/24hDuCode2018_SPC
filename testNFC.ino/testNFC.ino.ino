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

/* Exported define -----------------------------------------------------------*/
#define BULK_MAX_PACKET_SIZE            0x00000040

/* Regarding board antenna (and matching) appropriate 
value may be modified to optimized RF performances */
/* Analogue configuration register
 ARConfigB  bits  7:4 MOD_INDEX Modulation index to modulator
                      3:0 RX_AMP_GAIN Defines receiver amplifier gain
For type A you can also adjust the Timer Window
*/

/******************  PICC  ******************/
/* ISO14443A */
#define PICC_TYPEA_ACConfigA            0x27  /* backscaterring */

/* ISO14443B */
#define PICC_TYPEB_ARConfigD            0x0E  /* card demodulation gain */
#define PICC_TYPEB_ACConfigA            0x17  /* backscaterring */

/* Felica */
#define PICC_TYPEF_ACConfigA            0x17  /* backscaterring */

/* Private variables ---------------------------------------------------------*/

/* TT1 (PCD only)*/
uint8_t TT1Tag[NFCT1_MAX_TAGMEMORY];

/* TT2 */
uint8_t TT2Tag[NFCT2_MAX_TAGMEMORY];

/* TT3 */
uint8_t TT3Tag[NFCT3_MAX_TAGMEMORY];
uint8_t *TT3AttribInfo = TT3Tag, *TT3NDEFfile = &TT3Tag[NFCT3_ATTRIB_INFO_SIZE];

/* TT4 */
uint8_t CardCCfile      [NFCT4_MAX_CCMEMORY];
uint8_t CardNDEFfileT4A [NFCT4_MAX_NDEFMEMORY];
uint8_t CardNDEFfileT4B [NFCT4_MAX_NDEFMEMORY];

/* TT5 (PCD only)*/
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
  // 95HF HW Init
  ConfigManager_HWInit();

  // LED1
  pinMode(X_NUCLEO_NFC03A1_LED1, OUTPUT);

  // LED2
  pinMode(X_NUCLEO_NFC03A1_LED2, OUTPUT);

  // LED3
  pinMode(X_NUCLEO_NFC03A1_LED3, OUTPUT);

  // LED4
  pinMode(X_NUCLEO_NFC03A1_LED4, OUTPUT);
  
  // Configure USB serial interface
  SerialPort.begin(115200);
  
  SerialPort.print("\r\n\r\n---------------------------------------\r\n******Welcome to x-nucleo-nfc03a1 demo******\r\n----------------------------------------");
  SerialPort.print("\r\n\r\nPlease bring an NFC tag to the board vicinity and Press User Button B1 on the board to start URI Writer/Reader demo on the tag");
  
  terminal_msg_flag = true;

  digitalWrite(X_NUCLEO_NFC03A1_LED1, HIGH);
}


/* Loop ----------------------------------------------------------------------*/

void loop()
{
  devicemode = PCD;
    
  /* Scan to find if there is a tag */
  TagType = ConfigManager_TagHunting(TRACK_ALL);
    
  if(TagType==TRACK_NFCTYPE2)
  {
      TagDetected = true;
        
      if(terminal_msg_flag == true )
      {
        terminal_msg_flag = false ;
          
        /*---HT UI msg----------*/
        SerialPort.print( "\r\n\r\nTRACK_NFCTYPE2 NFC tag detected nearby");
        digitalWrite(X_NUCLEO_NFC03A1_LED2, HIGH);
      }
   }     
   else
    {
      TagDetected = false;
      
      if(terminal_msg_flag == false)
      {
        terminal_msg_flag = true ;
        /*---HT UI msg----------*/
        SerialPort.println( "\r\n\r\nCurrently there is no NFC tag in the vicinity");
        digitalWrite(X_NUCLEO_NFC03A1_LED2, LOW);
        digitalWrite(X_NUCLEO_NFC03A1_LED3, LOW);
        digitalWrite(X_NUCLEO_NFC03A1_LED4, LOW);
      }
    }
   
  delay(300);
    
  if (TagDetected == true)
  {       
    TagDetected = false;
      
      status = ERRORCODE_GENERIC;
      SerialPort.println( "Tag detecté ! Tentative de lecture :");
      delay(500);


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
  
            //char str[] ="- This, a sample string.";
            char * pch;
            //Serial.println ("Splitting string \"%s\" into tokens:\n",dataNFC);
            pch = strtok (dataNFC,":");
            while (pch != NULL)
            {
              printf ("%s\n",pch);
              pch = strtok (NULL, ":");
            }
           
            digitalWrite(X_NUCLEO_NFC03A1_LED4, HIGH);
          }
        
        }

      }
      Serial.println("Suite.");


      
        
      

    
  }
}

