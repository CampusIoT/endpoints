// From https://support.sodaq.com/sodaq-one/lorawan/

#include <Sodaq_RN2483.h>

#define debugSerial SerialUSB
#define loraSerial Serial2
#define serial Serial

unsigned char buffer[256];
int count=0;

void setup() {
  debugSerial.begin(9600);
  serial.begin(9600);
}

void loop() {
  if (serial.available())
    {
        while(serial.available())
        {
            buffer[count++]=serial.read();
            if(count == 256) break;
        }
        debugSerial.write(buffer,count);
        clearBufferArray();
        count = 0;
    }
}

int isGPSGP(unsigned char* trameGPS) {
  if(trameGPS[0] == '$' && trameGPS[1] == 'G' && trameGPS[2] == 'P')
    return 1;
  else
    return 0;
  }

int isGPSGPGGA(unsigned char* trameGPS) {
  if(trameGPS[0] == '$' && trameGPS[1] == 'G' && trameGPS[2] == 'P' && trameGPS[3] == 'G' && trameGPS[4] == 'G' && trameGPS[5] == 'A')
    return 1;
  else
    return 0;
  }


void clearBufferArray()
{
    for (int i=0; i<count;i++)
    { buffer[i]=NULL;}
}
