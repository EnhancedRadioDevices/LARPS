#include <SPI.h>
#include <RH_RF95.h>                          // We will eventually port this to LoRaHam library
#include "larps_packets.h"

RH_RF95 rf95;                                 // Setup RadioHead for SX1276 (HamShield: LoRa Edition)

float frequency = 432.250;

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to LARPS Protocol Analyzer");
    if (!rf95.init()) {                                                   // This inits the radio. The program will fail here if there's a communication issue
    Serial.println(F("SX1276 Failure: init failed."));                  // Print error messages
    Serial.println(F("Check hardware connections. Halting program."));  // Print error messages
    while(1) { }                                                        // Infinite loop if we fail, no sense in going on
  }
  rf95.setTxPower(23, false);                                           // This is how we set HamShield: LoRa Edition to 1 watt mode using RadioHead
  rf95.setFrequency(frequency);                                           // Set to an open auxiliary frequency
}

uint8_t packet_length;
int16_t rssi;
int16_t snr;
char text_buff[RH_RF95_MAX_MESSAGE_LEN];      // Buffer for text input
uint8_t packet[RH_RF95_MAX_MESSAGE_LEN];         // Buffer for incoming radio packets
uint8_t len = sizeof(packet);                    // Size of buffer
bool error_flag = false;

void loop() {
    if (rf95.available()) { 
         rf95.recv(packet, &len);
         rssi = rf95.lastRssi();
         snr = rf95.lastSNR();
         if(packet[0] == 0x01) { 
            error_flag = false;
            stamp(); Serial.print(" RX frequency:"); Serial.print(frequency); Serial.print(" rssi:"); Serial.print(rssi); Serial.print(" snr:"); Serial.print(snr);
            Serial.print(" Ver: "); Serial.print(packet[PKT_VERSION],DEC); Serial.print(" len:"); Serial.print(packet[PKT_LEN],DEC); packet_length = packet[PKT_LEN];
            Serial.print(" From:\""); for(int x = PKT_FROM; x < PKT_FROM+12; x++) {
              if(packet[x] < 32 | packet[x] > 127) { error_flag = true; Serial.print("<"); Serial.print(packet[x],HEX); Serial.print(">"); }
              else { if(packet[x] != ' ') { Serial.print((char)packet[x]); } }
            } 
            Serial.print("\" To:\""); for(int x = PKT_TO; x < PKT_TO+12; x++) { 
              if(packet[x] < 32 | packet[x] > 127) { error_flag = true; Serial.print("<"); Serial.print(packet[x],HEX); Serial.print(">"); }
              else {  if(packet[x] != ' ') { Serial.print((char)packet[x]); } }
            } 
            Serial.print("\" Relay:\""); for(int x = PKT_RELAY; x < PKT_RELAY+12; x++) {
              if(packet[x] < 32 | packet[x] > 127) { error_flag = true; Serial.print("<"); Serial.print(packet[x],HEX); Serial.print(">"); }
              else {  if(packet[x] != ' ') { Serial.print((char)packet[x]);  } }
            }
            Serial.print("\" Seq:"); Serial.print(packet[PKT_COUNTER],DEC);
            Serial.print(" TTL:"); Serial.print(packet[PKT_TTL],DEC);
            Serial.print(" Opt:"); printBinary(packet[PKT_OPTIONS]);
            Serial.print(" Type:0x"); if(packet[PKT_TYPE] < 16) { Serial.print("0"); } Serial.print(packet[PKT_TYPE],HEX); 
            Serial.print(" Data:\""); for(int x = PKT_DATA; x < packet_length-2; x++) {
              if(packet[x] < 32 | packet[x] > 127) { Serial.print("<"); Serial.print(packet[x],HEX); Serial.print(">"); }
              else { Serial.print((char)packet[x]); }
            }
            Serial.print("\" CRC:0x"); 
            if(packet[packet_length-1] <16) { Serial.print("0"); } Serial.print(packet[packet_length-1],HEX); 
            if(packet[packet_length] <16) { Serial.print("0"); } Serial.println(packet[packet_length],HEX); 
            if(error_flag == true) { stamp(); Serial.println("Prior packet contains malformed callsigns"); }
         }
         else { stamp(); Serial.print("Packet with invalid version received"); }
    }
}

void stamp() { 
  Serial.print("["); Serial.print(millis()); Serial.print("] "); 
}

void printBinary(byte inByte)
{
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(inByte, b));
  }
}
