#include <SPI.h>
#include <RH_RF95.h>                          // We will eventually port this to LoRaHam library
#include "larps_packets.h"

RH_RF95 rf95;                                 // Setup RadioHead for SX1276 (HamShield: LoRa Edition)

float frequency = 432.250;

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to LARPS Beacon");
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
uint32_t beacon_interval = 10000; 
String callsign = "KC7IBT      ";             // needs to be exactly 12 characters until we write a handler for sub 12 char calls
uint8_t packetcounter = 0;
uint8_t ttl = 5;
String beacon_text = "This is a test beacon"; 

void loop() {
    stamp(); Serial.print("Transmitting beacon...");
    transmitBeacon();
    Serial.println("done!");
    delay(beacon_interval);
}

void stamp() { 
  Serial.print("["); Serial.print(millis()); Serial.print("] "); 
}

void transmitBeacon() {
  uint8_t pktlen = 0;
  packet[PKT_VERSION] = L_VERSION; pktlen++;                    // Version of the packet from header file
  packet[PKT_LEN] = 0x00; pktlen++;                         // We don't know the size yet
  for(int x = PKT_FROM; x < PKT_FROM+12; x++) {
    packet[x] = callsign[x - PKT_FROM]; 
  } pktlen = pktlen + 12;
  for(int x = PKT_TO; x < PKT_TO+12; x++) { 
    if((x - PKT_TO) == 0) { packet[x] = '*'; } 
    else{ packet[x] = ' '; }
  } pktlen = pktlen + 12;
  for(int x = PKT_RELAY; x < PKT_RELAY+12; x++) {
    packet[x] = ' '; 
  } pktlen = pktlen + 12;
  packet[PKT_COUNTER] = packetcounter;
  packetcounter++; pktlen++;
  packet[PKT_TTL] = ttl;
  pktlen++;
  packet[PKT_OPTIONS] = 0;
  pktlen++;
  packet[PKT_TYPE] = L_TYPE_BEACON;
  pktlen++;
  for(int x = PKT_DATA; x < (PKT_DATA + beacon_text.length()+1); x++) 
  { 
    packet[x] = beacon_text[x-PKT_DATA];
    pktlen++;
  }
  packet[pktlen] = 0x00; 
  pktlen++;
  packet[pktlen] = 0x00;
  packet[PKT_LEN] = pktlen;
  Serial.print("packet size of "); Serial.print(pktlen,DEC); Serial.print("..");
  rf95.send(packet,pktlen);                     
  rf95.waitPacketSent(); 
  memset(packet,0,len);
}
