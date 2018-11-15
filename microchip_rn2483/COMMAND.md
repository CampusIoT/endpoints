# Commands for driving the RN2483 modem

## Install
You can install Coolterm http://freeware.the-meiers.org/ for send commands to the modem. The modem configuration must be 57600/8/N/1 and command lines must be terminated with <CR><LF>.

## LoRa Commands
The mac pause command must be called before any radio transmission or reception, even if no MAC operations have been initiated before.

### Read the modem information
  sys factoryRESET
  sys get ver
  sys get vdd
  sys get hweui


### Read the actual modem configuration
  mac pause
  radio get mod
  radio get freq
  radio get pwr
  radio get sf
  radio get afcbw
  radio get rxbw
  radio get fdev
  radio get prlen
  radio get iqi
  radio get wdt
  radio get bw
  radio get crc
  radio get cr
  radio get snr


### Configure the LoRa modem
  mac pause
  radio set bt 1
  radio set mod lora
  radio set freq 868100000
  radio set pwr 14
  radio set sf sf12
  radio set afcbw 125
  radio set rxbw 125
  radio set prlen 8
  radio set crc on
  radio set cr 4/5
  radio set sync 12
  radio set bw 125
  radio set wdt 2000

### Configure the LoRa modem for receiving frames (IQ inversion is OFF)
  mac pause
  radio set iqi off

### Start the reception
  radio rx 0


### Configure the LoRa modem for sending messages (IQ inversion is ON)
  mac pause
  radio set iqi on

### Send a frame
  radio tx C0534f5320444944494552

Remark: C0534f5320444944494552 is "SOS DIDIER" and C0 is a proprietary frame in the LoRaWAN specification

## LoRaWAN (Mac) Commands

### Set the EUIs and AppKey for OTAA

  mac set deveui 0004A30B001BA7BC                    
  mac set appeui FEDCBA9876543210
  mac set appkey 0004A30B001BA7BCFEDCBA9876543210        
  mac set pwridx 1


  sys get hweui

### Set the EUIs and AppKey for ABP

  mac set deveui 0004A30B001B44A6                    
  mac set appeui FEDCBA9876543210
  mac set appkey 0004A30B001B44A6FEDCBA9876543210        
  mac set appskey A660B08B52252A7B8F6B8D7FF3B6A7A4
  mac set nwkskey A660B08B52252A7B8F6B8D7FF3B6A7A4
  mac set devaddr 0C000014
  mac save
  mac join abp                
  mac tx cnf 1 534f5320444944494552
  mac tx uncnf 1 534f5320444944494552


  mac reset 868

  mac set deveui 0004A30B001BA7BC                    
  mac set appeui FEDCBA9876543210
  mac set appkey 0004A30B001BA7BCFEDCBA9876543210        

  mac set pwridx 1

  mac set dr 0
  mac set adr on                
  mac set bat 127                
  mac set retx 2
  mac set linkchk 100                    
  mac set rxdelay1 1000
  mac set ar on
  mac get rx2 868    
  mac set rx2 3 869525000

  mac get ch freq 0
  mac get ch freq 1
  mac get ch freq 2
  mac get ch dcycle 0
  mac get ch dcycle 1
  mac get ch dcycle 2

  > set DC to 50%
  mac set ch dcycle 0 1
  mac set ch dcycle 1 1                
  mac set ch dcycle 2 1                

  mac save

  mac join otaa

  → denied  

  mac tx cnf 1 01020304
  mac tx uncnf 1 0102034

## References
*   RN2483 LoRa Technology Module Command Reference User’s Guide http://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf 
