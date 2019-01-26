// From https://support.sodaq.com/sodaq-one/lorawan/

// This program requires the OneWire library
// Import it from Sketch > Include Libraries > Manage Libraries… and search "OneWire"

#include <Sodaq_RN2483.h>

#define debugSerial SerialUSB
#define loraSerial Serial2

#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)

// Delay between 2 transmissions
// 60000 = 1 minute
#define TX_PERIOD 60000

// #define PIR_MOTION_PIN 9

// TODO add #ifdef for OTAA/ABP in order to shrink the firmware
//Use OTAA, set to false to use ABP
bool OTAA = true;

// ABP
// USE YOUR OWN KEYS!
const uint8_t devAddr[4] =
{
    0x00, 0x00, 0x00, 0x00
};

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

// OTAA
// With using the GetHWEUI() function the HWEUI will be used
uint8_t DevEUI[8] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t AppEUI[8] =
{
  0x0c, 0xaf, 0xcb, 0x00, 0x00, 0xff, 0xff, 0xff
};

// 0400123428aed2a6abf7158809cf4f3c
const uint8_t AppKey[16] =
{
0x04, 0x00, 0x12, 0x34, 0x28, 0xae, 0xd2, 0xa6,
0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

void setup()
{
  delay(1000);

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
  }
  debugSerial.println();

  setupLoRa();
}

void setupLoRa(){
  if(!OTAA){
    // ABP
    setupLoRaABP();
  } else {
    //OTAA
    setupLoRaOTAA();
  }
  // Uncomment this line to for the RN2903 with the Actility Network
  // For OTAA update the DEFAULT_FSB in the library
  // LoRaBee.setFsbChannels(1);

  LoRaBee.setSpreadingFactor(9);
}

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

void setupLoRaOTAA(){

  if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
  {
    debugSerial.println("Network connection successful.");
  }
  else
  {
    debugSerial.println("Network connection failed!");
  }
}



/* Dépendance pour le bus 1-Wire */
#include <OneWire.h>


/* Broche du bus 1-Wire */
const byte BROCHE_ONEWIRE = 15;

/* Code de retour de la fonction getTemperature() */
enum DS18B20_RCODES {
  READ_OK,  // Lecture ok
  NO_SENSOR_FOUND,  // Pas de capteur
  INVALID_ADDRESS,  // Adresse reçue invalide
  INVALID_SENSOR  // Capteur invalide (pas un DS18B20)
};


/* Création de l'objet OneWire pour manipuler le bus 1-Wire */
OneWire ds(BROCHE_ONEWIRE);


/**
 * Fonction de lecture de la température via un capteur DS18B20.
 */
byte getTemperature(float *temperature, byte reset_search) {
  byte data[9], addr[8];
  // data[] : Données lues depuis le scratchpad
  // addr[] : Adresse du module 1-Wire détecté

  /* Reset le bus 1-Wire ci nécessaire (requis pour la lecture du premier capteur) */
  if (reset_search) {
    ds.reset_search();
  }

  /* Recherche le prochain capteur 1-Wire disponible */
  if (!ds.search(addr)) {
    // Pas de capteur
    return NO_SENSOR_FOUND;
  }

  /* Vérifie que l'adresse a été correctement reçue */
  if (OneWire::crc8(addr, 7) != addr[7]) {
    // Adresse invalide
    return INVALID_ADDRESS;
  }

  /* Vérifie qu'il s'agit bien d'un DS18B20 */
  if (addr[0] != 0x28) {
    // Mauvais type de capteur
    return INVALID_SENSOR;
  }

  /* Reset le bus 1-Wire et sélectionne le capteur */
  ds.reset();
  ds.select(addr);

  /* Lance une prise de mesure de température et attend la fin de la mesure */
  ds.write(0x44, 1);
  delay(800);

  /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande de lecture du scratchpad */
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);

 /* Lecture du scratchpad */
  for (byte i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  /* Calcul de la température en degré Celsius */
  *temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625;

  // Pas d'erreur
  return READ_OK;
}

int16_t getTemperatureInt16(float temperature){
  return (int16_t)(temperature*100);
}

void loop()
{
  float temperature;

  /* Lit la température ambiante à ~1Hz */
  if (getTemperature(&temperature, true) != READ_OK) {
    debugSerial.println(F("Error while reading temperature"));
    return;
  }

  int16_t temp = getTemperatureInt16(temperature);
  debugSerial.print("temperature:");
  debugSerial.println(temp/100.0);

//   switch (LoRaBee.send(1, (uint8_t*)reading.c_str(), reading.length()))
    switch (LoRaBee.send(1, (uint8_t*)&temp, sizeof(temp)))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! The program will reset the RN module.");
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. The program will reset the RN module.");
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. The program will reset the RN module.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      break;
    default:
      break;
    }
    // Delay between readings and transmissions
    delay(TX_PERIOD);
}

/**
* Gets and stores the LoRa module's HWEUI/
*/
static void getHWEUI()
{
  uint8_t len = LoRaBee.getHWEUI(DevEUI, sizeof(DevEUI));
}
