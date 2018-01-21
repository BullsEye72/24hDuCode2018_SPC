/*
#include <common.h>
#include <drv_95HF.h>
#include <drv_spi.h>
#include <lib_95HF.h>
#include <lib_95HFConfigManager.h>
#include <lib_iso14443A.h>
#include <lib_iso14443Apcd.h>
#include <lib_iso14443Bpcd.h>
#include <lib_iso15693pcd.h>
#include <lib_iso18092pcd.h>
#include <lib_iso7816pcd.h>
#include <lib_NDEF.h>
#include <lib_NDEF_AAR.h>
#include <lib_NDEF_Email.h>
#include <lib_NDEF_Geo.h>
#include <lib_NDEF_MyApp.h>
#include <lib_NDEF_SMS.h>
#include <lib_NDEF_Text.h>
#include <lib_NDEF_URI.h>
#include <lib_NDEF_Vcard.h>
#include <lib_nfctype1pcd.h>
#include <lib_nfctype2pcd.h>
#include <lib_nfctype3pcd.h>
#include <lib_nfctype4pcd.h>
#include <lib_nfctype5pcd.h>
#include <lib_pcd.h>
#include <lib_wrapper.h>
#include <miscellaneous.h>
*/

#include <SPI.h>
#include <WiFiST.h>
#include <PubSubClient.h>

#define SerialPort Serial
#define BULK_MAX_PACKET_SIZE            0x00000040

SPIClass SPI_3(PC12, PC11, PC10);
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);

char ssid[] = "24HDUCODE";         //  your network SSID (name)
char pass[] = "2018#24hcode!";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

const char* server = "24hducode.spc5studio.com";
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait for connection:
    delay(1000);
  }
  Serial.println("You're connected to the network");
  
  

}

void sendMessage2Broker(chr* message){
  Serial.println("connection to broker and send message");
  if(client.connect("teamC", "Psykokwak", "E1255A34")){
    client.subscribe("24hcode/teamC/7d253/broker2device");
    //client.publish("24hcode/teamC/7d253/device2broker","A1:Hello 24h du code!");    
    client.publish("24hcode/teamC/7d253/device2broker","A3a:A#BA#AA#AG#GG#GF#FF#FED#ED#DC#CC#DD#A");    
    
  }
}

void loop() {
  client.loop();
}
