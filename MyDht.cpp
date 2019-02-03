#include "MyDht.h"

MyDht::MyDht() {}

void MyDht::init() {
  dht = new DHT_Unified(11, DHT11);
  dht->begin();
  sensor_t sensor;
  dht->temperature().getSensor(&sensor);
  dht->humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void MyDht::update() {
  if (millis() - timing > delayMS) {
    sensors_event_t event;
    dht->temperature().getEvent(&event);
    temperatureChanged = currentTemperature != event.temperature;
    if(temperatureChanged) {
      currentTemperature = event.temperature;
    }
    dht->humidity().getEvent(&event);
    humidityChanged = currentHumidity != event.relative_humidity;
    if(humidityChanged) {
       currentHumidity = event.relative_humidity;
    }
    timing = millis();
  }
}

bool MyDht::isTemperatureChanged() {
  return temperatureChanged;
};
bool MyDht::isHumidityChanged() {
  return humidityChanged;
};
float MyDht::temperature() {
  return currentTemperature;
};
float MyDht::humidity() {
  return currentHumidity;
};

void MyDht::dropChanges() {
  temperatureChanged = false;
  humidityChanged = false;
}
