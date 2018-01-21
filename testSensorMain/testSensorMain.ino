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
#include <time.h>
#include <Wire.h>
#include <vl53l0x_class.h>

//====== MQTT ======================
#include <SPI.h>
#include <WiFiST.h>
#include <PubSubClient.h>
SPIClass SPI_3(PC12, PC11, PC10);
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);
char ssid[] = "24HDUCODE";         //  your network SSID (name)
char pass[] = "2018#24hcode!";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
const char* server = "24hducode.spc5studio.com";
WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}
PubSubClient client(server, 1883, callback, wifiClient);
//=======================================
//============ TOF =======================
TwoWire WIRE1(14, 15);  //SDA=PB11 & SCL=PB10
VL53L0X sensor_vl53l0x(&WIRE1, 4, 7); //XSHUT=PC6 & INT=PC7
int compteur = 0;


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
//uint8_t status = ERRORCODE_GENERIC;
static char dataOut[256];

#define X_NUCLEO_NFC03A1_LED1 D7
#define X_NUCLEO_NFC03A1_LED2 D6
#define X_NUCLEO_NFC03A1_LED3 D5
#define X_NUCLEO_NFC03A1_LED4 D4

Servo fs90r_L;
Servo fs90r_R;
int val_L;
int val_R;
int servo_L = 6;
int servo_R = 5;
int sensor_R = 0;
int sensor_L = 1;
int detect_SL;
int detect_SR;

int trackingState=0;
bool lost=false;

void setup()
{ 
 int status;
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

 
  // Initialize I2C bus.
  WIRE1.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X top component.
  status = sensor_vl53l0x.InitSensor(0x10);
  if(status)
  {
    Serial.println("Init sensor_vl53l0x failed...");
  }   
    
    // 95HF HW Init
  ConfigManager_HWInit();
  
  Serial.begin(115200);

  fs90r_L.attach(servo_L, 900, 2100); // attaches the servo on pin 9 to the servo object
  fs90r_R.attach(servo_R, 900, 2100); // Be carefull to min and max values..
  
  pinMode(sensor_R,INPUT);
  pinMode(sensor_L,INPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

   while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network - ");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}



void checkAndUpdateState()
{
  detect_SL=digitalRead(sensor_L);
  detect_SR=digitalRead(sensor_R);
//Serial.print(trackingState);
//Serial.print(lost);
//Serial.println("==========");

  if(trackingState == 0 && (detect_SL==LOW || detect_SR==LOW))
  {
    //Serial.println("0");
    trackingState=1;
    lost=false;
  }
  else if(detect_SL==HIGH || detect_SR==HIGH){
    //Serial.println("1");
    lost = true;
    if(detect_SL==HIGH)
    {
      trackingState=2;
     }
     else if(detect_SR==HIGH)
     {
      trackingState=3; 
      }
  }
  else if(lost && trackingState==2 && detect_SL==LOW){
    //Serial.println("2");
    lost=false;
    trackingState=1;
  }
  else if(lost && trackingState==3 && detect_SR==LOW){
    //Serial.println("3");
    lost=false;
    trackingState=1;
  }
  
  
  if(detect_SL==HIGH && detect_SR==HIGH){
    //Serial.println("4");
    trackingState=0;
  }  
  
}

void ApplyState(){
  int Rval = 90;
  int Lval = 90;

  int speedvalFast=18;
  int speedvalSlow=14;
  
  switch(trackingState)
  {
    case 1 : //TOUT DROIT !
      fs90r_L.write(Rval+speedvalFast);
      fs90r_R.write(Rval-speedvalFast);
      break;
    case 2 : //A GAUCHE !
      fs90r_L.write(Rval-speedvalSlow);
      fs90r_R.write(Rval-speedvalSlow);
      break;
    case 3 : //A DROITE !
      fs90r_L.write(Rval+speedvalSlow);
      fs90r_R.write(Rval+speedvalSlow);
      break;
    default: //STOP !
      fs90r_L.write(Rval);
      fs90r_R.write(Rval);
      break;
  }
}

char* areneId;
char* nomEpreuve;
char* instruction;

void nfctrucmuch(){
  
  TagType = ConfigManager_TagHunting(TRACK_ALL);
    
  if(TagType==TRACK_NFCTYPE2 && TagDetected == false)
  {
    TagDetected = true;
    SerialPort.println( "TAG NFC Detecté!");
  }     
  else
  {
    TagDetected = false;
    //Serial.println( "Plus de TAG NFC en vue...");
  }   

  if (TagDetected == true)
  {       
      status = ERRORCODE_GENERIC;
      SerialPort.print( "Tentative de lecture : ");
      delay(500);


      status = PCDNFCT2_ReadNDEF();
                
      if ( status == RESULTOK )
      {
        Serial.println("Lecture OK");
               
        status = NDEF_IdentifyNDEF( &RecordStruct, NDEF_Buffer);
          
        if(status == RESULTOK && RecordStruct.TypeLength != 0)
        {
          if (NDEF_ReadURI(&RecordStruct, &url)==RESULTOK) 
          {
            char* dataNFC = (char *)url.URI_Message;
            areneId = "";
            nomEpreuve = "";
            instruction = "";
              
            char * pch;
            pch = strtok (dataNFC,":");
            while (pch != NULL)
            {
              if(areneId==""){
                areneId=pch;
              }
              else if(nomEpreuve==""){
                nomEpreuve=pch;
              }
              else if(instruction==""){
                instruction=pch;
              }
              
              pch = strtok (NULL, ":");
            }
           
          }
        
        }

      }
      
      TagDetected = false;
    }
    
}

void loopTracking(){
  checkAndUpdateState();
  ApplyState();
}

//===================== MUSIC ! ============================
#define  C     3830    // 261 Hz
#define  D     3400    // 294 Hz
#define  E     3038    // 329 Hz
#define  F     2864    // 349 Hz 
#define  G     2550    // 392 Hz 
#define  A     2272    // 440 Hz 
#define  B     2028    // 493 Hz 
#define  R     20

void chanter(char* tab)
{
  long duration  = 0;
  int rest_count = 100;
  int speaker = A5;
  long tempo = 10000;
  int pause = 1000; 
  int tone_ = 0;
  int beat = 0;
  //int songLength = sizeof(tab) / sizeof(char)-1;  
  //int songLength = 20;  
  int songLength = strlen(tab);
  int melody[songLength];
  int beats[songLength];

  pinMode(speaker, OUTPUT);
 
  Serial.print("ON EST ICI : ");
  Serial.println(songLength);
  Serial.println(instruction);

  for(int i = 0; i < songLength; i++)
  {
    if(tab[i] == 'C')
    {
      melody[i] = C;
      beats[i] = 32;
    }
    else if(tab[i] == 'D')
    {
      melody[i] = D;
      beats[i] = 32;
    }
    else if(tab[i] == 'E')
    {
      melody[i] = E;
      beats[i] = 32;
    }
    else if(tab[i] == 'F')
    {
      melody[i] = F;
      beats[i] = 32;
    }
    else if(tab[i] == 'G')
    {
      melody[i] = G;
      beats[i] = 32;
    }
    else if(tab[i] == 'A')
    {
      melody[i] = A;
      beats[i] = 32;
    }
    else if(tab[i] == 'B')
    {
      melody[i] = B;
      beats[i] = 32;
    }
    else if(tab[i] == '#')
    {
      melody[i] = R;
      beats[i] = 32;
    }
  }

  for (int i=0; i<songLength; i++) 
  {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo;

    long elapsed_time = 0;
    if (tone_ > 0) 
    { 
      while (elapsed_time < duration) 
      {
        digitalWrite(speaker,HIGH);
        delayMicroseconds(tone_ / 2);

        // DOWN
        digitalWrite(speaker, LOW);
        delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
        elapsed_time += (tone_);
      } 
    }
    else 
    {
      for (int j = 0; j < rest_count; j++) 
      {
        delayMicroseconds(duration);  
      }                                
    } 
    
    delayMicroseconds(pause);
   } 
}

//==========================================================
bool isPetted = false;
bool isStuck = false;

void readRange(){
// Read Range.
  uint32_t distance;
  int status;
  status = sensor_vl53l0x.GetDistance(&distance);

  //Serial.println(distance);

  if(distance < 130)
  {
     if(status == 0)
    {
      compteur++;
      Serial.print("Nb. de caresses : ");
      Serial.println(compteur);
    }

    if(compteur >= 5)
    {
      Serial.println("Merci! Je vais mieux!");
      isPetted=false;
      compteur = 0;
    }

    if (status == VL53L0X_ERROR_NONE)
    {
      // Output data.
      //char report[64];
      //snprintf(report, sizeof(report), "| Distance [mm]: %ld |", distance);
      //Serial.println(report);
    }
  }
}
//==========================================================


void loop()
{

    for(int i = 0 ; i < 40 ; i++){
      loopTracking();
      delay(5);
    } 
    
    trackingState=0;
    ApplyState();
    //delay(3000);
    nfctrucmuch();

    if(strcmp(areneId,"")!=0){
      Serial.print("ARENE A FAIRE : ");
      Serial.println(areneId);
      Serial.print("NOM ARENE : ");
      Serial.println(nomEpreuve);
      Serial.print("INSTRUCTION : ");
      Serial.println(instruction);
    }

    if(strcmp(areneId,"A1") == 0){
        epreuveA1();
    }
    else if(strcmp(areneId,"A2") == 0){
        epreuveA2();
    }
    else if(strcmp(areneId,"A3a") == 0){
        epreuveA3();
    }
    else if(strcmp(areneId,"A4a") == 0){
        //FONCTION;
    }
    else if(strcmp(areneId,"A5a") == 0){
        epreuveA5();
    }
    else if(strcmp(areneId,"A6") == 0){
        epreuveA6();
    }
    else if(strcmp(areneId,"A8") == 0){
        epreuveA8();
    }

    areneId="";
}

void epreuveA1()
{
  Serial.println("connection to broker for A1");
  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.publish("24hcode/teamC/7d253/device2broker","A1:Hello 24h du code!");
    client.subscribe("24hcode/teamC/7d253/broker2device");
  }
  Serial.println("Message envoyé!");
}

void epreuveA2()
{
  Serial.println("connection to broker for A2");

  char tmpChar[50] = "A2:";
  strcat(tmpChar,instruction);

  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.publish("24hcode/teamC/7d253/device2broker", tmpChar);
  }
  Serial.println("Message envoyé!");
}

void epreuveA3(){
   
  chanter(instruction);
  //Serial.println("OHHHH SOLE MIIOOOO !!!");
  //delay(4000);
  
  char tmpChar[50] = "A3a:";
  strcat(tmpChar,instruction);
  
  Serial.println("connection to broker for A3");
  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.publish("24hcode/teamC/7d253/device2broker", tmpChar);
  }
  Serial.println("Message envoyé!");
  Serial.println(tmpChar);
}

void epreuveA6(){
  

  isPetted = true;
  while(isPetted){
    readRange();  
  }  
  
  Serial.println("connection to broker for A6");
  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.publish("24hcode/teamC/7d253/device2broker", "A6:5");
  }
  Serial.println("Message envoyé!");
}

void epreuveA8(){
  int servo_tof = 3;
  Servo fs90_tof;
  fs90_tof.attach(servo_tof, 900, 2100);

  for(int r = 0 ; r < 5 ; r++){
    fs90_tof.write(200);
    delay(500);
    fs90_tof.write(50);
    delay(500);
  }
  fs90_tof.write(130);
    
  Serial.println("connection to broker for A8");
  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.publish("24hcode/teamC/7d253/device2broker", "A8:blue");
  }
  Serial.println("Message envoyé!");
}




void epreuveA5(){
    char message[100], ch;
    int i, key;

    Serial.println("connection to broker for A5");
     
    for(key = 1; key<26 ; key++){
      strcpy(message,instruction);
      
      for(i = 0; message[i] != '\0'; ++i){
          ch = message[i];
          
          if(ch >= 'a' && ch <= 'z'){
              ch = ch - key;
              
              if(ch < 'a'){
                  ch = ch + 'z' - 'a' + 1;
              }
              
              message[i] = ch;
          }
          else if(ch >= 'A' && ch <= 'Z'){
              ch = ch - key;
              
              if(ch < 'A'){
                  ch = ch + 'Z' - 'A' + 1;
              }
              
              message[i] = ch;
          }
      }

      char tmpChar[50] = "A5a:";
      strcat(tmpChar,message);
      
      if(client.connect("teamC", "Psykokwak", "E1255A34")){
        client.publish("24hcode/teamC/7d253/device2broker",tmpChar);
      }
    }
      
  Serial.println("Message envoyé!");
}

