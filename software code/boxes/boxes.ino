#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define SOILPIN A3
#define CE 9 // Пины NRF
#define CSN 10 // Пины NRF
#define ARDUINO_NUM 1

DHT_Unified dht(DHTPIN, DHTTYPE);

const uint64_t pipe = 0xF0F1F2F3F4LL;
RF24 radio(CE,CSN);

void setup() {
  radio.begin();
  radio.setChannel(0);
  delay(2);

  radio.setChannel(0);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);

  radio.openWritingPipe(pipe);
  radio.stopListening();
  
  dht.begin();
}

void loop() {
  sensors_event_t event;
  
  float data[4] = {ARDUINO_NUM, 0, 0, 0};

  dht.temperature().getEvent(&event);
  data[1] = event.relative_humidity;

  dht.humidity().getEvent(&event);
  data[2] = event.temperature;
  
  data[3] = analogRead(SOILPIN);

  radio.write(&data, sizeof(data));

  delay(300);
}
