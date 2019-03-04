// A simple LARPS Beacon that also transmits the repeater directory for the local area

#include <SPI.h>
#include <RH_RF95.h>                          // We will eventually port this to LoRaHam library
#include "larps_packets.h"

RH_RF95 rf95;                                 // Setup RadioHead for SX1276 (HamShield: LoRa Edition)

float frequency = 432.250;                    // LARPS frequency
uint32_t beacon_interval = 10;                // LARPS beacon interval in seconds
String beacon_text = "Visit http://enhancedradio.com/LARPS/ to learn more about this service"; // LARPS beacon contents
String callsign = "KC7IBT      ";             // needs to be exactly 12 characters until we write a handler for sub 12 char calls


// The repeater format is: call sign/frequency/offset in khz/tone in/tone out/city/flags
// flags can be grouped if more than one exists:
// O = Open
// C = Closed
// E = Emergency Power (restricted)
// A = Autopatch available
// L = Repeater link avaliable
// C = Echolink/Internet linked
// X = offline

#define ARRAY_SIZE 5                          // Number of repeaters in the list

String repeaters[ARRAY_SIZE] = { 
    "W2CMC/147.2/+600/146.2/146.2/CMCH/O",
    "N2ICV/443.05/+5000/DMR/DMR/CMCH/O",
    "W2CMC/443.6/+5000/146.2/146.2/CMCH/O",
    "NJ2DS/447.475/-5000/0/0/CMCH/O",
    "N3JCS/447.7625/-5000/DMR/DMR/CMCH/OX"
};


void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to LARPS Beacon");
    if (!rf95.init()) {                                                 // This inits the radio. The program will fail here if there's a communication issue
    Serial.println(F("SX1276 Failure: init failed."));                  // Print error messages
    Serial.println(F("Check hardware connections. Halting program."));  // Print error messages
    while(1) { }                                                        // Infinite loop if we fail, no sense in going on
  }
  rf95.setTxPower(23, false);                                           // This is how we set HamShield: LoRa Edition to 1 watt mode using RadioHead
  rf95.setFrequency(frequency);                                         // Set to an open auxiliary frequency
  rf95.setCADTimeout(10000);                                               // Channel busy timeout of 10 seconds
  beacon_interval = beacon_interval * 1000;                             // change beacon interval into milliseconds
}

uint8_t packet_length;
int16_t rssi;
int16_t snr;
char text_buff[RH_RF95_MAX_MESSAGE_LEN];      // Buffer for text input
uint8_t packet[RH_RF95_MAX_MESSAGE_LEN];         // Buffer for incoming radio packets
uint8_t len = sizeof(packet);                    // Size of buffer
bool error_flag = false;
uint8_t packetcounter = 0;
uint8_t ttl = 5;

void loop() {
    stamp(); Serial.println("Transmitting beacon and repeater directory");
    transmitBeacon();
    transmitRepeaterDirectory();
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
  packet[PKT_TYPE] = L_BEACON;
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
  stamp();
  Serial.print("Sending beacon [");
  Serial.print(beacon_text);
  Serial.print("] ");
  Serial.print("packet size of "); Serial.print(pktlen,DEC); Serial.print("..");
  if(rf95.waitCAD() == true) { 
      rf95.send(packet,pktlen);                     
      rf95.waitPacketSent();
      Serial.println("sent!");
  }
  else { Serial.print("...Couldn't transmit packet!"); } 
  memset(packet,0,len);
}


void transmitRepeaterDirectory() {
  for(int r = 0; r < ARRAY_SIZE; r++) { 
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
  packet[PKT_TYPE] = L_REPEATER_DIRECTORY;
  pktlen++;
  for(int x = PKT_DATA; x < (PKT_DATA + repeaters[r].length()+1); x++) 
  { 
    packet[x] = repeaters[r][x-PKT_DATA];
    pktlen++;
  }
  packet[pktlen] = 0x00; 
  pktlen++;
  packet[pktlen] = 0x00;
  packet[PKT_LEN] = pktlen;
  stamp();
  Serial.print("Sending repeater "); Serial.print(r,DEC); Serial.print(" ["); Serial.print(repeaters[r]);
  Serial.print("] packet size of "); Serial.print(pktlen,DEC); Serial.print("..");
  if(rf95.waitCAD() == true) { 
  rf95.send(packet,pktlen);                     
  rf95.waitPacketSent(); Serial.println("sent");
  } else { Serial.print("...Couldn't transmit packet!"); } 
  memset(packet,0,len); RNG();
  }
}

uint32_t random_count;
uint32_t seed;

void RNG() {
  random_count = analogRead(A0);
  for(int x = 0; x < random_count; x++) { 
    //digitalWrite(A1,analogRead(A2) % 2);
    seed = analogRead(A0);
  }
  seed = seed ^ micros();
  randomSeed(seed);
}
