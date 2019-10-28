// SODAQ Explorer shetch with :
// * OTAA or ABP network activation
// * onboard temperature sensor
// * Grove LCD Display (on I2C socket)
// * RGB led

// From https://support.sodaq.com/sodaq-one/lorawan/

#include <Sodaq_RN2483.h>

// Use OTAA, comment to use ABP
#define OTAA

// Use LCD, comment to disable lcd
#define GROVE_RGB_LCD  

#ifdef GROVE_RGB_LCD
// Refer to http://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/ for installing the library
#include <rgb_lcd.h>
# endif

#define debugSerial SerialUSB
#define loraSerial Serial2

#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)

// Delay between 2 transmissions
// 60000 = 1 minute
#define TX_PERIOD 60000

// #define PIR_MOTION_PIN 9



const uint8_t devAddr[4] =
{
    0x00, 0x00, 0x00, 0x00
};

// With using the GetHWEUI() function the HWEUI will be used
uint8_t DevEUI[8] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#ifdef OTAA

// OTAA
const uint8_t AppEUI[8] =
{
  0x0c, 0xaf, 0xcb, 0x00, 0x00, 0xff, 0xff, 0xff
};

// 09cf4f3c0400123428aed2a6abf71588
const uint8_t AppKey[16] =
{
0x09, 0xcf, 0x4f, 0x3c, 0x04, 0x00, 0x12, 0x34,
0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88
};

#else

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#endif

#ifdef GROVE_RGB_LCD
rgb_lcd lcd;

#define WHITE 255,255,255
#define BLACK 0,0,0
#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255

#endif

void setup()
{
  delay(1000);
#ifdef GROVE_RGB_LCD
  lcd.begin(16, 2);
  lcd.setRGB(WHITE);
  lcd.print("LORA");
#endif

  while ((!debugSerial) && (millis() < 10000)){
    // Wait 10 seconds for debugSerial to open
  }

  debugSerial.println("Start");

  // Start streams
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  LoRaBee.setDiag(debugSerial); // to use debug remove //DEBUG inside library
  LoRaBee.init(loraSerial, LORA_RESET);

  //Use the Hardware EUI
  getHWEUI();

  // Print the Hardware EUI
  debugSerial.print("LoRa HWEUI: ");
  for (uint8_t i = 0; i < sizeof(DevEUI); i++) {
    debugSerial.print((char)NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(DevEUI[i])));
    debugSerial.print((char)NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(DevEUI[i])));
#ifdef GROVE_RGB_LCD
    lcd.print((char)NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(DevEUI[i])));
    lcd.print((char)NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(DevEUI[i])));
#endif
  }
  debugSerial.println();  

  setupLoRa();
}

#ifdef OTAA
void setupLoRaOTAA(){
  if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
  {
    debugSerial.println("Network connection successful.");
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
#ifdef GROVE_RGB_LCD
    lcd.setCursor(0, 1);
    lcd.setRGB(GREEN);
    lcd.print("OTAA success");
#endif
  }
  else
  {
    debugSerial.println("Network connection failed!");
#ifdef GROVE_RGB_LCD
    lcd.setCursor(0, 1);
    lcd.setRGB(RED);
    lcd.print("OTAA failed");  
#endif
  }
  delay(1000);
}
#else
void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}
#endif

void setupLoRa(){
#ifdef OTAA
    //OTAA
    setupLoRaOTAA();
#else
    // ABP
    setupLoRaABP();
#endif
  // Uncomment this line to for the RN2903 with the Actility Network
  // For OTAA update the DEFAULT_FSB in the library
  // LoRaBee.setFsbChannels(1);

  LoRaBee.setSpreadingFactor(9);
}

void loop()
{
  //String reading = getTemperature();
  int16_t temp = getTemperatureInt16();
  debugSerial.print("temperature:");
  debugSerial.println(temp/100.0);
#ifdef GROVE_RGB_LCD
  lcd.setCursor(0, 0);
  lcd.setRGB(BLUE);
  lcd.print("T:");
  lcd.print(temp/100.0);
  lcd.setCursor(0, 1);
#endif  
//   switch (LoRaBee.send(1, (uint8_t*)reading.c_str(), reading.length()))
    switch (LoRaBee.send(1, (uint8_t*)&temp, sizeof(temp)))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Success");
#endif
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX NoResponse");
#endif
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Timeout");
#endif
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Size Error");
#endif
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! The program will reset the RN module.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Internal Err");
#endif
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Busy");
#endif
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. The program will reset the RN module.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX Fatal");
#endif
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. The program will reset the RN module.");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX NotConnected");
#endif
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
#ifdef GROVE_RGB_LCD    
      lcd.print("TX NoAck");
#endif
      break;
    default:
      break;
    }
    // Delay between readings and transmissions
    delay(TX_PERIOD);
}

String getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = (mVolts - 500.0) / 10.0;

  return String(temp);
}
// TODO for Cayenne LPP
// TODO https://github.com/myDevicesIoT/cayenne-docs/blob/master/docs/LORA.md
// TODO https://github.com/aabadie/cayenne-lpp
int16_t getTemperatureInt16()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  int16_t temp = (int16_t) (((mVolts - 500.0) / 10.0) * 100);

  return temp;
}

float getTemperatureFloat()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = ((mVolts - 500.0) / 10.0);

  return temp;
}

// TODO add battery level in payload
// TODO send on button push
// TODO status byte (button flag, presence flag...)
// TODO get ADC grove connector value (potentiometer for instance)
// TODO add a PIR motion detector on pin 9 and add a presence counter into the payload
// TODO get GNSS module geolocation value on RX/TX pins. See http://forum.sodaq.com/search?q=GPS
// TODO add downlink (for TX_PERIOD configuration, number of readings per transmission, low and high temperature thresholds)
// TODO enable several readings per transmission
// TODO add Grove LCD display


/**
* Gets and stores the LoRa module's HWEUI/
*/
static void getHWEUI()
{
  uint8_t len = LoRaBee.getHWEUI(DevEUI, sizeof(DevEUI));
}


#ifdef LED_ENABLED
/**
* set RGB led to red
*/
void ledRED() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
* set RGB led to green
*/
void ledGREEN() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

/**
* set RGB led to blue
*/
void ledBLUE() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

/**
* set RGB led to white
*/
void ledWHITE() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

/**
* set RGB led to off
*/
void ledOFF() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void setupRGBLED() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}
#endif
