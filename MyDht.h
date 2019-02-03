#ifndef __MY_DHT11_H__
  #define __MY_DHT11_H__

  #include <DHT.h>
  #include <DHT_U.h>


class MyDht {
  private:
    DHT_Unified* dht;
    unsigned long timing = 0;
    uint32_t delayMS;
    float currentTemperature = 0;
    float currentHumidity = 0;
    bool temperatureChanged = false;
    bool humidityChanged = false;
  public:
    MyDht();
    void init();
    void update();
    bool isTemperatureChanged();
    bool isHumidityChanged();
    float temperature();
    float humidity();
    void dropChanges();
};

#endif
