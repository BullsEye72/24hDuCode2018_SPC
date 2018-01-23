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
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(200);

  client.loop();
}
