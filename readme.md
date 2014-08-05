  # Ultimate RCSwitch Library 
  # Raspberry Pi library for remote control outlet switches
  
  * Made by Remi Sarrailh : Gpl v2.1 (this is a fork)
  * Link : https://github.com/maditnerd/UltimateRCswitch
  * Original Project home: http://code.google.com/p/rc-switch/

  # Protocol handle by this Libraries
  - RCSWITCH1 : Default RCSwitch Protocol works with most power outlets
  - RCSWITCH2 : Not tested, documentation stated it works with Conrad RSL88R/B
  - RCSWITCH3 : Not tested, documentation stated it works with BL35P02
  - VENUS     : Rising Sun RSL887T1 (Low cost ceiling lamp switch named as venus) http://www.risingsun-elec.com/ProductOne.asp?ID=160
  - CHACON    : Idleman implementation of Home Easy for chacon devices

  # Credits
  
  ## Original Author (Many Many Kudos for this amazing works!)
  - Suat Özgür : Gpl v2.1 Copyright (c) 2011 .  All right reserved.

  ## Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com*
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
  - Dominik Fischer / dom_fischer(at)web(dot)de
  - Frank Oltmanns
  - Andreas Steinel 
  - Remi Sarrailh (maditnerd) / maditnerd(at)gmail(dot)com
  - Valentin Carruesco (idleman) / idleman(at)idleman(dot).fr
  
  

  # Licence

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

  # How to compile a program with this
  ````g++ RCsend.cpp RCSwitch.cpp -o RCsend -lwiringPi````

  Have fun and happy Hacking!

