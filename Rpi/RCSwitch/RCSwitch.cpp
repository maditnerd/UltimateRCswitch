/*
  Ultimate RCSwitch Library - Raspberry Pi libary for remote control outlet switches
  Made by Remi Sarrailh : Gpl v2.1 (this is a fork)
  Link : https://github.com/maditnerd/UltimateRCswitch

  Protocol handle by this Libraries
  ---------------------------------
  RCSWITCH1 : Default RCSwitch Protocol works with most power outlets
  RCSWITCH2 : Not tested, documentation stated it works with Conrad RSL88R/B
  RCSWITCH3 : Not tested, documentation stated it works with BL35P02
  VENUS     : Rising Sun RSL887T1 (Low cost ceiling lamp switch named as venus) http://www.risingsun-elec.com/ProductOne.asp?ID=160
  CHACON    : Idleman implementation of Home Easy for chacon devices

  Credits
  -------

  Original Author (Many Many Kudos for this amazing works!)
  - Suat Özgür : Gpl v2.1 Copyright (c) 2011 .  All right reserved.

  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com*
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
  - Dominik Fischer / dom_fischer(at)web(dot)de
  - Frank Oltmanns
  - Andreas Steinel 
  - Remi Sarrailh (maditnerd) / maditnerd(at)gmail(dot)com
  - Valentin Carruesco (idleman) / idleman(at)idleman(dot).fr
  
  Original Project home: http://code.google.com/p/rc-switch/

  Licence
  -------

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  How to compile a program with this
  ----------------------------------
  g++ RCsend.cpp RCSwitch.cpp -o RCsend -lwiringPi

  Have fun and happy Hacking!

*/

#include "RCSwitch.h"
  
  unsigned long RCSwitch::nReceivedValue = NULL;
  unsigned int RCSwitch::nReceivedBitlength = 0;
  unsigned int RCSwitch::nReceivedDelay = 0;
  unsigned int RCSwitch::nReceivedProtocol = 0;
  unsigned int RCSwitch::timings[RCSWITCH_MAX_CHANGES];
  int RCSwitch::nReceiveTolerance = 60;

  RCSwitch::RCSwitch() {
    this->nReceiverInterrupt = -1;
    this->nTransmitterPin = -1;
    RCSwitch::nReceivedValue = NULL;
    this->setPulseLength(350);
    this->setRepeatTransmit(10);
    this->setReceiveTolerance(60);
    this->setProtocol(RCSWITCH1);
  }

/**
  * Sets the protocol to send.
  */
  void RCSwitch::setProtocol(int nProtocol) {
    this->nProtocol = nProtocol;
    if (nProtocol == RCSWITCH1){
      this->setPulseLength(350);
    }
    else if (nProtocol == RCSWITCH2) {
     this->setPulseLength(650);
   }
   else if (nProtocol == RCSWITCH3) {
    this->setPulseLength(100);
  }
  else if (nProtocol == CHACON) {
    this->setPulseLength(275);
  }
  else if (nProtocol == VENUS) {
    this->setPulseLength(235);
  }
}

/**
  * Sets the protocol to send with pulse length in microseconds.
  */
  void RCSwitch::setProtocol(int nProtocol, int nPulseLength) {
    this->nProtocol = nProtocol;
    this->setPulseLength(nPulseLength);
 }


/**
  * Sets pulse length in microseconds
  */
  void RCSwitch::setPulseLength(int nPulseLength) {
    this->nPulseLength = nPulseLength;
  }

/**
 * Sets Repeat Transmits
 */
 void RCSwitch::setRepeatTransmit(int nRepeatTransmit) {
  this->nRepeatTransmit = nRepeatTransmit;
}

/**
 * Set Receiving Tolerance
 */
 void RCSwitch::setReceiveTolerance(int nPercent) {
  RCSwitch::nReceiveTolerance = nPercent;
}


/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
 void RCSwitch::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
}

/**
  * Disable transmissions
  */
  void RCSwitch::disableTransmit() {
    this->nTransmitterPin = -1;
  }

/**
 * Switch a remote switch on (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
  */
 void RCSwitch::switchOn(char sFamily, int nGroup, int nDevice) {
  this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, true) );
}

/**
 * Switch a remote switch off (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
 */
 void RCSwitch::switchOff(char sFamily, int nGroup, int nDevice) {
  this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, false) );
}

/**
 * Switch a remote switch on (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
 void RCSwitch::switchOn(int nAddressCode, int nChannelCode) {
  this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, true) );
}

/**
 * Switch a remote switch off (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
 void RCSwitch::switchOff(int nAddressCode, int nChannelCode) {
  this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, false) );
}

/**
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..4)
 */
 void RCSwitch::switchOn(char* sGroup, int nChannel) {
  this->sendTriState( this->getCodeWordA(sGroup, nChannel, true) );
}

/**
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..4)
 */
 void RCSwitch::switchOff(char* sGroup, int nChannel) {
  this->sendTriState( this->getCodeWordA(sGroup, nChannel, false) );
}

/**
 * Returns a char[13], representing the Code Word to be send.
 * A Code Word consists of 9 address bits, 3 data bits and one sync bit but in our case only the first 8 address bits and the last 2 data bits were used.
 * A Code Bit can have 4 different states: "F" (floating), "0" (low), "1" (high), "S" (synchronous bit)
 *
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 * | 4 bits address (switch group) | 4 bits address (switch number) | 1 bit address (not used, so never mind) | 1 bit address (not used, so never mind) | 2 data bits (on|off) | 1 sync bit |
 * | 1=0FFF 2=F0FF 3=FF0F 4=FFF0   | 1=0FFF 2=F0FF 3=FF0F 4=FFF0    | F                                       | F                                       | on=FF off=F0         | S          |
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 * @param bStatus       Wether to switch on (true) or off (false)
 *
 * @return char[13]
 */
 char* RCSwitch::getCodeWordB(int nAddressCode, int nChannelCode, boolean bStatus) {
   int nReturnPos = 0;
   static char sReturn[13];
   
   const char* code[5] = { "FFFF", "0FFF", "F0FF", "FF0F", "FFF0" };
   if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4) {
    return '\0';
  }
  for (int i = 0; i<4; i++) {
   sReturn[nReturnPos++] = code[nAddressCode][i];
 }

 for (int i = 0; i<4; i++) {
   sReturn[nReturnPos++] = code[nChannelCode][i];
 }

 sReturn[nReturnPos++] = 'F';
 sReturn[nReturnPos++] = 'F';
 sReturn[nReturnPos++] = 'F';

 if (bStatus) {
  sReturn[nReturnPos++] = 'F';
} else {
  sReturn[nReturnPos++] = '0';
}

sReturn[nReturnPos] = '\0';

return sReturn;
}


/**
 * Like getCodeWord  (Type A)
 */
 char* RCSwitch::getCodeWordA(char* sGroup, int nChannelCode, boolean bStatus) {
   int nReturnPos = 0;
   static char sReturn[13];

   const char* code[6] = { "FFFFF", "0FFFF", "F0FFF", "FF0FF", "FFF0F", "FFFF0" };

   if (nChannelCode < 1 || nChannelCode > 5) {
    return '\0';
  }
  
  for (int i = 0; i<5; i++) {
    if (sGroup[i] == '0') {
      sReturn[nReturnPos++] = 'F';
    } else if (sGroup[i] == '1') {
      sReturn[nReturnPos++] = '0';
    } else {
      return '\0';
    }
  }
  
  for (int i = 0; i<5; i++) {
    sReturn[nReturnPos++] = code[ nChannelCode ][i];
  }
  
  if (bStatus) {
    sReturn[nReturnPos++] = '0';
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = 'F';
    sReturn[nReturnPos++] = '0';
  }
  sReturn[nReturnPos] = '\0';

  return sReturn;
}

/**
 * Like getCodeWord (Type C = Intertechno)
 */
 char* RCSwitch::getCodeWordC(char sFamily, int nGroup, int nDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;
  
  if ( (byte)sFamily < 97 || (byte)sFamily > 112 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4) {
    return '\0';
  }
  
  char* sDeviceGroupCode =  dec2binWzerofill(  (nDevice-1) + (nGroup-1)*4, 4  );
  char familycode[16][5] = { "0000", "F000", "0F00", "FF00", "00F0", "F0F0", "0FF0", "FFF0", "000F", "F00F", "0F0F", "FF0F", "00FF", "F0FF", "0FFF", "FFFF" };
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = familycode[ (int)sFamily - 97 ][i];
  }
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = (sDeviceGroupCode[3-i] == '1' ? 'F' : '0');
  }
  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  if (bStatus) {
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = '0';
  }
  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * Sends a Code Word
 * @param sCodeWord   /^[10FS]*$/  -> see getCodeWord
 */
 void RCSwitch::sendTriState(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
        this->sendT0();
        break;
        case 'F':
        this->sendTF();
        break;
        case '1':
        this->sendT1();
        break;
      }
      i++;
    }
    this->sendSync();    
  }
}

/*
Home Easy Sender
 */
void RCSwitch::send(unsigned long remote, unsigned long button, boolean onoff){
  int lastprotocol = this->nProtocol;
  int lastRepeatTransmit = this->nRepeatTransmit;

  setProtocol(4);
  setRepeatTransmit(1);

  char* remote_bin = this->dec2binWzerofill(remote, 26);
  char* button_bin = this->dec2binWzerofill2(button, 4);
  //Serial.println(this->dec2binWzerofill(remote, 26));
//char* remote_bin = (this->dec2binWcharfill(remote, 26, '0'));
//char* button_bin = (this->dec2binWcharfill(button, 4, '0'));
  //Serial.println(remote_bin);
  //Serial.println(button_bin);
  //Set Protocol to Home Easy (Chacon/DIO) 
  //Latch1
  this->transmit(1,36);
  //Latch2
  this->transmit(1,10);
  digitalWrite(this->nTransmitterPin, HIGH);
  
  //Remote (Recipient Code)
  this->send(remote_bin);

  //No Group
  this->sendPair(false);

  //ON/OFF
  this->sendPair(onoff);

  //Button (Sender Code)
  this->send(button_bin);

  //End Message
  this->send0();
  delay(10);

  setProtocol(lastprotocol);
  setRepeatTransmit(lastRepeatTransmit);

}

//RCSwitch sender
void RCSwitch::send(unsigned long Code, unsigned int length) {
  this->send( this->dec2binWzerofill(Code, length) );
}

void RCSwitch::send(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
     

      if(i == 0 && this->nProtocol == VENUS){ //NO MATTERS WHAT VENUS FIRST BIT IS A 1
        this->send1();
      }
      else
      {
      
        if(this->nProtocol != CHACON){ //Use send0 if not chacon (homeeasy)
          switch(sCodeWord[i]) {
            case '0':
            this->send0();
            break;
            case '1':
            this->send1();
            break;
          }
        }
        else if(this->nProtocol == CHACON){ //Use sendPair if chacon (homeeasy)
         switch(sCodeWord[i]) {
          case '0':
          this->sendPair(false);
          break;
          case '1':
          this->sendPair(true);
          break;
        }
      }
    }
    i++;
  }
  this->sendSync();
}
}

void RCSwitch::transmit(int nHighPulses, int nLowPulses) {
  boolean disabled_Receive = false;
  int nReceiverInterrupt_backup = nReceiverInterrupt;
  if (this->nTransmitterPin != -1) {
    if (this->nReceiverInterrupt != -1) {
      this->disableReceive();
      disabled_Receive = true;
    }
    digitalWrite(this->nTransmitterPin, HIGH);
    delayMicroseconds( this->nPulseLength * nHighPulses);
    digitalWrite(this->nTransmitterPin, LOW);
    delayMicroseconds( this->nPulseLength * nLowPulses);
    if(disabled_Receive){
      this->enableReceive(nReceiverInterrupt_backup);
    }
  }
}
/**
 * Sends a "0" Bit
 *                       _    
 * Waveform Protocol 1: | |___
 *                       _  
 * Waveform Protocol 2: | |__
 */
 void RCSwitch::send0() {
   if (this->nProtocol == RCSWITCH1){
    this->transmit(1,3);
  }
  else if (this->nProtocol == RCSWITCH2) {
    this->transmit(1,2);
  }
  else if (this->nProtocol == RCSWITCH3) {
        this->transmit(4,11);
    }
  else if (this->nProtocol == CHACON){
    this->transmit(1,1);
  }
  else if (this->nProtocol == VENUS) {
    this->transmit(2.52,5.70);
  }
}

/**
 * Sends a "1" Bit
 *                       ___  
 * Waveform Protocol 1: |   |_
 *                       __  
 * Waveform Protocol 2: |  |_
 */
 void RCSwitch::send1() {
   if (this->nProtocol == RCSWITCH1){
    this->transmit(3,1);
  }
  else if (this->nProtocol == RCSWITCH2) {
    this->transmit(2,1);
  }
  else if (this->nProtocol == RCSWITCH3){
    this->transmit(9,6);
  }
  else if (this->nProtocol == CHACON) {
    this->transmit(1,5);
  }
   else if (this->nProtocol == VENUS) {
    this->transmit(6.914,1);
  }
}


/**
 * Sends a Tri-State "0" Bit
 *            _     _
 * Waveform: | |___| |___
 */
 void RCSwitch::sendT0() {
  this->transmit(1,3);
  this->transmit(1,3);
}

/**
 * Sends a Tri-State "1" Bit
 *            ___   ___
 * Waveform: |   |_|   |_
 */
 void RCSwitch::sendT1() {
  this->transmit(3,1);
  this->transmit(3,1);
}

/**
 * Sends a Tri-State "F" Bit
 *            _     ___
 * Waveform: | |___|   |_
 */
 void RCSwitch::sendTF() {
  this->transmit(1,3);
  this->transmit(3,1);
}

/**
 * Sends a "Sync" Bit
 *                       _
 * Waveform Protocol 1: | |_______________________________
 *                       _
 * Waveform Protocol 2: | |__________
 */
 void RCSwitch::sendSync() {

  if (this->nProtocol == RCSWITCH1){
    this->transmit(1,31);
  }
  else if (this->nProtocol == RCSWITCH2) {
    this->transmit(1,10);
  }
   else if (this->nProtocol == RCSWITCH3) {
        this->transmit(1,71);
    }
  else if (this->nProtocol == VENUS) {
   this->transmit(3.28,27.70);

 }
}

void RCSwitch::sendPair(boolean b) {
  if(b)
 {
   this->send1();
   this->send0();
 }
 else
 {
  this->send0();
  this->send1();
 }
}



/**
 * Enable receiving data
 */
 void RCSwitch::enableReceive(int interrupt) {
  this->nReceiverInterrupt = interrupt;
  this->enableReceive();
}

void RCSwitch::enableReceive() {
  if (this->nReceiverInterrupt != -1) {
    RCSwitch::nReceivedValue = NULL;
    RCSwitch::nReceivedBitlength = NULL;
    wiringPiISR(this->nReceiverInterrupt, INT_EDGE_BOTH, &handleInterrupt);
  }
}

/**
 * Disable receiving data
 */
 void RCSwitch::disableReceive() {
  this->nReceiverInterrupt = -1;
}

bool RCSwitch::available() {
  return RCSwitch::nReceivedValue != NULL;
}

void RCSwitch::resetAvailable() {
  RCSwitch::nReceivedValue = NULL;
}

unsigned long RCSwitch::getReceivedValue() {
  return RCSwitch::nReceivedValue;
}

unsigned int RCSwitch::getReceivedBitlength() {
  return RCSwitch::nReceivedBitlength;
}

unsigned int RCSwitch::getReceivedDelay() {
  return RCSwitch::nReceivedDelay;
}

unsigned int RCSwitch::getReceivedProtocol() {
  return RCSwitch::nReceivedProtocol;
}

unsigned int* RCSwitch::getReceivedRawdata() {
  return RCSwitch::timings;
}

/**
 *
 */
 bool RCSwitch::receiveProtocol1(unsigned int changeCount){

   unsigned long code = 0;
   unsigned long delay = RCSwitch::timings[0] / 31;
   unsigned long delayTolerance = delay * RCSwitch::nReceiveTolerance * 0.01;    

   for (int i = 1; i<changeCount ; i=i+2) {

    if (RCSwitch::timings[i] > delay-delayTolerance && RCSwitch::timings[i] < delay+delayTolerance && RCSwitch::timings[i+1] > delay*3-delayTolerance && RCSwitch::timings[i+1] < delay*3+delayTolerance) {
      code = code << 1;
    } else if (RCSwitch::timings[i] > delay*3-delayTolerance && RCSwitch::timings[i] < delay*3+delayTolerance && RCSwitch::timings[i+1] > delay-delayTolerance && RCSwitch::timings[i+1] < delay+delayTolerance) {
      code+=1;
      code = code << 1;
    } else {
            // Failed
      i = changeCount;
      code = 0;
    }
  }      
  code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      RCSwitch::nReceivedValue = code;
      RCSwitch::nReceivedBitlength = changeCount / 2;
      RCSwitch::nReceivedDelay = delay;
      RCSwitch::nReceivedProtocol = 1;
    }

    if (code == 0){
      return false;
    }else if (code != 0){
      return true;
    }


  }

  bool RCSwitch::receiveProtocol2(unsigned int changeCount){

   unsigned long code = 0;
   unsigned long delay = RCSwitch::timings[0] / 10;
   unsigned long delayTolerance = delay * RCSwitch::nReceiveTolerance * 0.01;    

   for (int i = 1; i<changeCount ; i=i+2) {

    if (RCSwitch::timings[i] > delay-delayTolerance && RCSwitch::timings[i] < delay+delayTolerance && RCSwitch::timings[i+1] > delay*2-delayTolerance && RCSwitch::timings[i+1] < delay*2+delayTolerance) {
      code = code << 1;
    } else if (RCSwitch::timings[i] > delay*2-delayTolerance && RCSwitch::timings[i] < delay*2+delayTolerance && RCSwitch::timings[i+1] > delay-delayTolerance && RCSwitch::timings[i+1] < delay+delayTolerance) {
      code+=1;
      code = code << 1;
    } else {
            // Failed
      i = changeCount;
      code = 0;
    }
  }      
  code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      RCSwitch::nReceivedValue = code;
      RCSwitch::nReceivedBitlength = changeCount / 2;
      RCSwitch::nReceivedDelay = delay;
      RCSwitch::nReceivedProtocol = 2;
    }

    if (code == 0){
      return false;
    }else if (code != 0){
      return true;
    }

  }

  /** Protocol 3 is used by BL35P02.
 *
 */
bool RCSwitch::receiveProtocol3(unsigned int changeCount){
    
      unsigned long code = 0;
      unsigned long delay = RCSwitch::timings[0] / PROTOCOL3_SYNC_FACTOR;
      unsigned long delayTolerance = delay * RCSwitch::nReceiveTolerance * 0.01;    

      for (int i = 1; i<changeCount ; i=i+2) {
      
          if  (RCSwitch::timings[i]   > delay*PROTOCOL3_0_HIGH_CYCLES - delayTolerance
            && RCSwitch::timings[i]   < delay*PROTOCOL3_0_HIGH_CYCLES + delayTolerance
            && RCSwitch::timings[i+1] > delay*PROTOCOL3_0_LOW_CYCLES  - delayTolerance
            && RCSwitch::timings[i+1] < delay*PROTOCOL3_0_LOW_CYCLES  + delayTolerance) {
            code = code << 1;
          } else if (RCSwitch::timings[i]   > delay*PROTOCOL3_1_HIGH_CYCLES - delayTolerance
                  && RCSwitch::timings[i]   < delay*PROTOCOL3_1_HIGH_CYCLES + delayTolerance
                  && RCSwitch::timings[i+1] > delay*PROTOCOL3_1_LOW_CYCLES  - delayTolerance
                  && RCSwitch::timings[i+1] < delay*PROTOCOL3_1_LOW_CYCLES  + delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
      if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
        RCSwitch::nReceivedValue = code;
        RCSwitch::nReceivedBitlength = changeCount / 2;
        RCSwitch::nReceivedDelay = delay;
        RCSwitch::nReceivedProtocol = 3;
      }

      if (code == 0){
        return false;
      }else if (code != 0){
        return true;
      }
}


  void RCSwitch::handleInterrupt() {

    static unsigned int duration;
    static unsigned int changeCount;
    static unsigned long lastTime;
    static unsigned int repeatCount;

    long time = micros();
    duration = time - lastTime;

    if (duration > 5000 && duration > RCSwitch::timings[0] - 200 && duration < RCSwitch::timings[0] + 200) {    
      repeatCount++;
      changeCount--;

      if (repeatCount == 2) {
        if (receiveProtocol1(changeCount) == false){
          if (receiveProtocol2(changeCount) == false){
                                //failed
          }
        }
        repeatCount = 0;
      }
      changeCount = 0;
    } else if (duration > 5000) {
      changeCount = 0;
    }

    if (changeCount >= RCSWITCH_MAX_CHANGES) {
      changeCount = 0;
      repeatCount = 0;
    }
    RCSwitch::timings[changeCount++] = duration;
    lastTime = time;  
  }

/**
  * Turns a decimal value to its binary representation
  */
  char* RCSwitch::dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
    static char bin[64];
    unsigned int i=0;

    while (Dec > 0) {
      bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
      Dec = Dec >> 1;
    }

    for (unsigned int j = 0; j< bitLength; j++) {
      if (j >= bitLength - i) {
        bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
      }else {
        bin[j] = '0';
      }
    }
    bin[bitLength] = '\0';

    return bin;
  }

/**
  * Turns a decimal value to its binary representation
  */
  char* RCSwitch::dec2binWzerofill2(unsigned long Dec, unsigned int bitLength){
    static char bin[64];
    unsigned int i=0;

    while (Dec > 0) {
      bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
      Dec = Dec >> 1;
    }

    for (unsigned int j = 0; j< bitLength; j++) {
      if (j >= bitLength - i) {
        bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
      }else {
        bin[j] = '0';
      }
    }
    bin[bitLength] = '\0';

    return bin;
  }

char* RCSwitch::dec2binWcharfill2(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';
  return bin;
}

char* RCSwitch::dec2binWcharfill(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';
  return bin;
}