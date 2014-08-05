/*

RFsend hacked from codesend by Sarrailh Remi

 'codesend' hacked from 'send' by @justy
 
 - The provided rc_switch 'send' command uses the form systemCode, unitCode, command
   which is not suitable for our purposes.  Instead, we call 
   send(code, length); // where length is always 24 and code is simply the code
   we find using the RF_sniffer.ino Arduino sketch.

 Usage: ./RFsend transmitterPin decimalcode
 (Use RF_Sniffer.ino to check that RF signals are being produced by the RPi's transmitter)
 */

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>



 int main(int argc, char *argv[]) {

  if (wiringPiSetup () == -1) return 13;

  
  RCSwitch radio = RCSwitch();
  radio.enableTransmit(11);
  //radio.setProtocol(4);
  printf("SENDING RADIO CODE TO VENUS\n");
  radio.send(10151594,0,false);
  delay(10000);
  radio.send(10151594,0,false);
  delay(10000);
 

 return 0;


}