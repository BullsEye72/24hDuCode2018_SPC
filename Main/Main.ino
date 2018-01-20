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
#include <Servo.h>

//=============== MACHINE STATE VARIABLES ==================
Servo fs90r_L;
Servo fs90r_R;
int servo_L = 6; //pins
int servo_R = 5; //pins
int sensor_R = 0; //pins
int sensor_L = 1; //pins
int detect_SL;
int detect_SR;
int speedval=20;
int turnFact=2;
int trackingState=0;
bool lost=false;
//=============== END MACHINE STATE VARIABLES ==================
//=============== NFC VARIABLES ==================
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
//=============== END NFC VARIABLES ==================

//=================== MACHINE STATE FUNCTIONS ========================
void ApplyState(){
  switch(trackingState)
  {
    case 1 : //TOUT DROIT !
      fs90r_L.write(90+speedval);
      fs90r_R.write(90-speedval);
      break;
    case 2 : //A GAUCHE !
      fs90r_L.write(90+speedval*turnFact);
      fs90r_R.write(90+speedval*turnFact);
      break;
    case 3 : //A DROITE !
      fs90r_L.write(90-speedval*turnFact);
      fs90r_R.write(90-speedval*turnFact);
      break;
    default: //STOP !
      fs90r_L.write(90);
      fs90r_R.write(90);
      break;
  }
}

void checkAndUpdateState()
{
  detect_SL=digitalRead(sensor_L);
  detect_SR=digitalRead(sensor_R);

  if(trackingState == 0 && (detect_SL==LOW || detect_SR==LOW))
  {
    Serial.println("0");
    trackingState=1;
    lost=false;
    return;
  }

  if(!lost && (detect_SL==HIGH || detect_SR==HIGH)){
    Serial.println("1");
    lost = true;
    if(detect_SL==HIGH) { trackingState=3; }
    if(detect_SR==HIGH) { trackingState=2; }
    return;
  }

  if(lost && trackingState==2 && detect_SL==LOW){
    Serial.println("2");
    lost=false;
    trackingState=1;
    return;
  }

  if(lost && trackingState==3 && detect_SR==LOW){
    Serial.println("3");
    lost=false;
    trackingState=1;
    return;
  }

  if(detect_SL==HIGH && detect_SR==HIGH){
    Serial.println("4");
    trackingState=0;
  }
 }
 //===================== END MACHINE STATE FUNCTIONS ======================== 
 //===================== NFC READING FUNCTIONS ==============================
void processNFC()
{
  devicemode = PCD;
  TagType = ConfigManager_TagHunting(TRACK_ALL);
  TagDetected = TagType==TRACK_NFCTYPE2;

  if (TagDetected == true)
  {       
    TagDetected = false;
      
    status = ERRORCODE_GENERIC;
    SerialPort.println( "Tag detecté ! Tentative de lecture :");

    trackingState=0;
    
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
  //====================== END NFC FUNCTIONS ===================================


void setup() {
  Serial.begin(115200);
  
  Serial.println("SETUP WHEEL SERVOS ...");
  fs90r_L.attach(servo_L, 900, 2100); // Be carefull to min and max values..
  fs90r_R.attach(servo_R, 900, 2100);
  pinMode(sensor_R,INPUT);
  pinMode(sensor_L,INPUT);
  
  Serial.println("SETUP NFC ...");
  ConfigManager_HWInit();  
  terminal_msg_flag = true;
  digitalWrite(X_NUCLEO_NFC03A1_LED1, HIGH);
}

void loop() {
  
  checkAndUpdateState();

  //processNFC();
  
  ApplyState();

}
