#include <Arduino.h>
// Fix Parameters
// Possible Values for Serial_Print_Mode  ! DONT TOUCH !
//
// This is a tesprogram for ESP32 boards with Lora
//
// It uses parts of the multigeiger program
//  I tested it in Jan 2020 successfully
// in the userdefines.h some selection must be made.
// Also check patformio.ini file to select the correct board



#define   Serial_None            0  // No Serial Printout
#define   Serial_Debug           1  // Only debug and error output will be printed via RS232(USB)
#define   Serial_Logging         2  // Log measurement as table via RS232(USB)
#define   Serial_One_Minute_Log  3  // One Minute logging will be printed via RS232(USB)
#define   Serial_Statistics_Log  4  // Lists time between two events in us via RS232(USB)

// Usable CPU-Types
// WIFI -> Heltev Wifi Kit 32
#define WIFI 0
// LORA  ->  Heltec Wifi Lora 32 (V2)
#define LORA 1
// STICK ->  Heltec Wireless Stick  (has LoRa on board)
#define STICK 2
//
// Includes
//====================================================================================================================================
#include "userdefines.h"

//====================================================================================================================================
#include <U8x8lib.h>

char          ssid[30];

// for LoRa

#include "loraWan.h"


unsigned long getESPchipID() {
  uint64_t espid = ESP.getEfuseMac();
  uint8_t *pespid = (uint8_t*)&espid;
  uint32_t id = 0;
  uint8_t *pid = (uint8_t *)&id;
  pid[0] = (uint8_t)pespid[5];
  pid[1] = (uint8_t)pespid[4];
  pid[2] = (uint8_t)pespid[3];
  Serial.printf("ID: %08X\n", id);
  Serial.printf("MAC: %04X%08X\n",(uint16_t)(espid>>32),(uint32_t)espid);
  return id;
}
// LoRa payload:
// to minimise airtime, we only send necessary bytes. We do NOT use Cayenne LPP.
// The payload will be translated via http integration and a small python program
// to be compatible with luftdaten.info. For byte definitions see ttn2luft.pdf in
// docs directory

void sendData2TTN(int sendwhat, unsigned int hvpulses) {
  Serial.println("sendData2TTN");
  unsigned char ttnData[20];
  ttnData[0]= 0x01;
  ttnData[1]= 0x02;
  ttnData[2]= 0x03;
  int cnt;
  cnt = 6;
  lorawan_send(1,ttnData,cnt,false,NULL,NULL,NULL);
}




void setup()
{
  // OLED-Display
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("Let's go!");
  uint32_t xx = getESPchipID();

  // build SSID
  sprintf(ssid,"ESP32-%d",xx);
  Serial.println(ssid);
  
  // init LoRa
  lorawan_setup();


} // end of setup



void loop()
{


    #if SEND2DUMMY
    displayStatusLine(F("Toilet"));
    Serial.println("SENDING TO TOILET");
  
    sendData2http(TOILET,SEND_CPM,hvp,true);
    
    delay(300);
    #endif

    // if we are LoRa, then send datas to TTN
   
    Serial.println("Sending to TTN ...");
    sendData2TTN(100,200);
    // print state of switch
    Serial.println("data sent");
	delay(5000);

}