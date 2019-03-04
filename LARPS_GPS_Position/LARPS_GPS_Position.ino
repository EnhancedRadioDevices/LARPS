// LARPS Position
// Connect to a NMEA GPS and provide position updates to the LARPS network
// Uses hardware serial with most GPS receivers. Neo 6M GPS: https://www.enhancedradio.com/products/ublox-neo-6m-gps-receiver-with-antenna
// Solder GPS to the HamShield: LoRa Edition board. 
// VCC to 3.3V pin, GND to GND pin, and TX to pin 0 (Arduino RX). You will need to remove HamShield: LoRa Edition during programming of the Uno
// Raspberry Pi can use default serial port without any programming conflicts
// Works with Adafruit GPS library

#include <SPI.h>                              // Required for RadioHead
#include <RH_RF95.h>                          // We will eventually port this to LoRaHam library
#include "larps_packets.h"                    // Defines for packet structure
#include <Adafruit_GPS.h>                     // GPS library

// Tell Adafruit GPS library how to connect to our GPS

#define GPSSerial Serial
Adafruit_GPS GPS(&GPSSerial);

// Create radio object

RH_RF95 rf95;                                 // Setup RadioHead for SX1276 (HamShield: LoRa Edition)

// Configuration options for tracking device

String callsign = "KC7IBT      ";             // needs to be exactly 12 characters until we write a handler for sub 12 char calls
#define STATION_TYPE S_CAR                    // Type of station
uint32_t position_update_interval = 60;       // Update interval for positions in seconds
float frequency = 432.250;                    // Frequency

// Variables

uint8_t packet_length;
int16_t rssi;
int16_t snr;
char text_buff[RH_RF95_MAX_MESSAGE_LEN];      // Buffer for text input
uint8_t packet[RH_RF95_MAX_MESSAGE_LEN];         // Buffer for incoming radio packets
uint8_t len = sizeof(packet);                    // Size of buffer
bool error_flag = false;

uint8_t packetcounter = 0;
uint8_t ttl = 5;
String position = ""; 
uint32_t timer = millis() + position_update_interval;


// Setup routine

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to LARPS Position Client");
    if (!rf95.init()) {                                                   // This inits the radio. The program will fail here if there's a communication issue
    Serial.println(F("SX1276 Failure: init failed."));                  // Print error messages
    Serial.println(F("Check hardware connections. Halting program."));  // Print error messages
    while(1) { }                                                        // Infinite loop if we fail, no sense in going on
  }
  rf95.setTxPower(23, false);                                           // This is how we set HamShield: LoRa Edition to 1 watt mode using RadioHead
  rf95.setFrequency(frequency);                                           // Set to an open auxiliary frequency
  stamp(); Serial.print("Transmission interval set to "); 
  Serial.print(position_update_interval,DEC); Serial.println(" Seconds");
  stamp(); Serial.println("Collecting GPS data");
  position_update_interval = position_update_interval * 1000;
}

// Main GPS tracking routine

void loop() {
    char c = GPS.read();
      if (GPS.newNMEAreceived()) {
            GPS.parse(GPS.lastNMEA());
            if(timer < millis()) { 
               timer = millis() + position_update_interval;
               stamp(); Serial.println("Transmission interval reached");
               position = String(STATION_TYPE,DEC)+ "," + GPS.hour + ":" + GPS.minute + ":" + GPS.seconds +","+ GPS.day + "/" + GPS.month+ "/" + GPS.year +",";
               position += String(GPS.fix,DEC) + "," + String(GPS.fixquality,DEC) + ",";
               if(GPS.fix) { 
                position += String(GPS.latitudeDegrees,4) + "," + String(GPS.longitudeDegrees,4) + "," + GPS.speed + "," + GPS.angle + ","+ GPS.altitude + "," + String(GPS.satellites,DEC);
               }
               else { position += "NO_POSITION"; }
               stamp(); Serial.print("Message contents: "); Serial.println(position);
               transmitPositionUpdate();
            }
      }
}

void stamp() { 
  Serial.print("["); Serial.print(millis()); Serial.print("] "); 
}

void transmitPositionUpdate() {
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
  packet[PKT_TYPE] = L_POSITION_UPDATE;
  pktlen++;
  for(int x = PKT_DATA; x < (PKT_DATA + position.length()+1); x++) 
  { 
    packet[x] = position[x-PKT_DATA];
    pktlen++;
  }
  packet[pktlen] = 0x00; 
  pktlen++;
  packet[pktlen] = 0x00;
  packet[PKT_LEN] = pktlen;
  stamp(); Serial.print("packet size of "); Serial.print(pktlen,DEC); Serial.print("..");
  rf95.send(packet,pktlen);                     
  rf95.waitPacketSent();
  Serial.println("sent!"); 
  memset(packet,0,len);
}

// Activity spinner (disabled since Arduino Serial Monitor doesn't support control characters

char spinner[4] = "|/-\\";
int spinptr = 0;
void spin() { 
  Serial.print("\b"); Serial.print((char)spinner[spinptr]); 
  spinptr++;
  if(spinptr > sizeof(spinner)) { spinptr = 0; } 
}
