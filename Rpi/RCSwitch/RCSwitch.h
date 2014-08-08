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

#ifndef _RCSwitch_h
#define _RCSwitch_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <wiringPi.h>
    #include <stdint.h>
    #define NULL 0
    #define CHANGE 1
    #define RCSWITCH1 1 // Default protocol, should works with must stuff
    #define RCSWITCH2 2 // I never used Protocol 2 so I don't know what it is about 
    #define RCSWITCH3 3 // Protocol 3 is missing from original library
    #define CHACON 4
    #define VENUS 5 //Venus Low Cost Switch for Radio Controlled Lamp
#ifdef __cplusplus
  extern "C"{
#endif
    typedef uint8_t boolean;
    typedef uint8_t byte;

#if !defined(NULL)
#endif
#ifdef __cplusplus
  }
#endif
#endif


// Number of maximum High/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES 67

#define PROTOCOL3_SYNC_FACTOR   71
#define PROTOCOL3_0_HIGH_CYCLES  4
#define PROTOCOL3_0_LOW_CYCLES  11
#define PROTOCOL3_1_HIGH_CYCLES  9
#define PROTOCOL3_1_LOW_CYCLES   6

  class RCSwitch {

  public:
    RCSwitch();
    
    void switchOn(int nGroupNumber, int nSwitchNumber);
    void switchOff(int nGroupNumber, int nSwitchNumber);
    void switchOn(char* sGroup, int nSwitchNumber);
    void switchOff(char* sGroup, int nSwitchNumber);
    void switchOn(char sFamily, int nGroup, int nDevice);
    void switchOff(char sFamily, int nGroup, int nDevice);

    void sendTriState(char* Code);
    void send(unsigned long remote, unsigned long button, boolean onoff);
    void send(unsigned long Code, unsigned int length);
    void send(char* Code);
    
    void enableReceive(int interrupt);
    void enableReceive();
    void disableReceive();
    bool available();
    void resetAvailable();

    unsigned long getReceivedValue();
    unsigned int getReceivedBitlength();
    unsigned int getReceivedDelay();
    unsigned int getReceivedProtocol();
    unsigned int* getReceivedRawdata();

    void enableTransmit(int nTransmitterPin);
    void disableTransmit();
    void setPulseLength(int nPulseLength);
    void setRepeatTransmit(int nRepeatTransmit);
    void setReceiveTolerance(int nPercent);
    void setProtocol(int nProtocol);
    void setProtocol(int nProtocol, int nPulseLength);

  private:
    char* getCodeWordB(int nGroupNumber, int nSwitchNumber, boolean bStatus);
    char* getCodeWordA(char* sGroup, int nSwitchNumber, boolean bStatus);
    char* getCodeWordC(char sFamily, int nGroup, int nDevice, boolean bStatus);
    void sendT0();
    void sendT1();
    void sendTF();
    void send0();
    void send1();
    void sendSync();
    void sendPair(boolean b);
    void transmit(int nHighPulses, int nLowPulses);

    static char* dec2binWzerofill(unsigned long dec, unsigned int length);
    static char* dec2binWzerofill2(unsigned long dec, unsigned int length);
    static char* dec2binWcharfill(unsigned long dec, unsigned int length, char fill);
    static char* dec2binWcharfill2(unsigned long dec, unsigned int length, char fill);
    
    static void handleInterrupt();
    static bool receiveProtocol1(unsigned int changeCount);
    static bool receiveProtocol2(unsigned int changeCount);
    static bool receiveProtocol3(unsigned int changeCount);
    int nReceiverInterrupt;
    int nTransmitterPin;
    int nPulseLength;
    int nRepeatTransmit;
    char nProtocol;

    static int nReceiveTolerance;
    static unsigned long nReceivedValue;
    static unsigned int nReceivedBitlength;
    static unsigned int nReceivedDelay;
    static unsigned int nReceivedProtocol;
    static unsigned int timings[RCSWITCH_MAX_CHANGES];

    
  };

#endif
