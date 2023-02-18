#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define WATERING_PIN 14
#define HUMIDIFIER_PIN 15
#define SOIL_PIN A4
 
RF24 radio(9, 10);
const uint64_t pipe = 0xF0F1F2F3F4LL;

bool enableWatering = false;
unsigned long wateringTime = 0;

bool enableHumidifier = false;
unsigned long humidifierTime = 0;

const float MIN_AIR_HUMIDIRY = 35;
const float MIN_SOIL_HUMIDITY = 400;
const float WATERING_TIME = 10000;
const float HUMIDIFIER_TIME = 5000;

int serial_putc( char c, FILE * ) {
  Serial.write( c );
  return c;
}

void printf_begin(void) {
  fdevopen( &serial_putc, 0 );
}
 
void setup() {
  Serial.begin(9600);
  printf_begin();
  radio.printDetails();
  
  radio.begin();
  radio.setChannel(0);
  delay(2);

  radio.setChannel(0); 
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  
  radio.startListening();

  radio.openReadingPipe(1, pipe);
  radio.printDetails();

  pinMode(WATERING_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
}
 
void loop() {
  if (enableWatering && millis() - wateringTime > WATERING_TIME) {
    enableWatering = false;
    digitalWrite(WATERING_PIN, LOW);
  }

  if (enableHumidifier && millis() - humidifierTime > HUMIDIFIER_TIME) {
    enableHumidifier = false;
    digitalWrite(HUMIDIFIER_PIN, LOW);
  }
  
  if (radio.available()) {
    float data[4];
    radio.read(&data, sizeof(data));

    Serial.println(data[1]);
    Serial.println(data[2]);
    Serial.println(data[3]);
    Serial.println();

    if (data[3] < MIN_SOIL_HUMIDITY && data[3] != 0 && !enableWatering) {
      digitalWrite(WATERING_PIN, HIGH);
      enableWatering = true;
      wateringTime = millis();
    }

    if (data[2] < MIN_AIR_HUMIDIRY && data[2] != 0 && !enableHumidifier) {
      digitalWrite(HUMIDIFIER_PIN, HIGH);
      enableHumidifier = true;
      humidifierTime = millis();
    }

    delay(500);
  }
}
