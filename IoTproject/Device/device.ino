#include "AZ3166WiFi.h"
#include "Sensor.h"

static DevI2C *ext_i2c;
static HTS221Sensor *tempSensor;
static float temperature;
static float humidity;


static boolean hasWifi = false;

static void initWifi()
{
  Screen.clean();
  Screen.print(2, "Connecting...");

  if (WiFi.begin() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running.... \r\n");
  }
  else {
    hasWifi = false;
    Screen.print(1, "No Wifi \r\n");
  }
}

void setup() {
  // put your setup code here, to run once:
  Screen.init();
  Screen.print(1, "Initializing...");

  Screen.print(2, "> Serial: ");
  Serial.begin(115200);

  Screen.print(3, "> WiFi");
  hasWifi = false;
  initWifi();
  if (hasWifi == false) {return;}

  ext_i2c = new DevI2C(D14, D15);
  Screen.print(3, "> Temperature sensor");
  tempSensor = new HTS221Sensor(*ext_i2c);
  tempSensor->init(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (hasWifi) {
    tempSensor->enable();
    tempSensor->getTemperature(&temperature);
    Serial.printf("Temperature %f\n", temperature);

    tempSensor->getHumidity(&humidity);
    Serial.printf("Humidity %f\n", humidity);

    tempSensor->disable();
    tempSensor->reset();
  }
  delay(5000);

}
