#include "AZ3166WiFi.h"
#include "Sensor.h"
#include "AzureIotHub.h"
#include "DevKitMQTTClient.h"
#include "RGB_LED.h"

#define RGB_LED_BRIGHTNESS 32

static DevI2C *ext_i2c;
static HTS221Sensor *tempSensor;
static LPS22HBSensor *pressureSensor;
static LIS2MDLSensor *magneticSensor;
static LSM6DSLSensor *accGyroSensor;

static RGB_LED rgbLed;

static float temperature;
static float humidity;
static float pressure;

static boolean hasWifi = false;
static boolean hasIoTHub = false;
int counter = 0;

const char *msg = "{\"temperature\": %f, \"humidity\": %f, \"pressure\": %f}";

void blinkLED()
{
    rgbLed.turnOff();
    rgbLed.setColor(0, 0, RGB_LED_BRIGHTNESS);
    delay(500);
    rgbLed.turnOff();
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    blinkLED();
  }
}

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
  
  delay(5000); // because Azure needs some time 
  if (!DevKitMQTTClient_Init()) 
  {
    hasIoTHub = false;
    return;
  }
  hasIoTHub = true;
  DevKitMQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  
  ext_i2c = new DevI2C(D14, D15);
  tempSensor = new HTS221Sensor(*ext_i2c);
  tempSensor->init(NULL);

  pressureSensor = new LPS22HBSensor(*ext_i2c);
  pressureSensor->init(NULL);

  magneticSensor = new LIS2MDLSensor(*ext_i2c);
  magneticSensor->init(NULL);

  accGyroSensor = new LSM6DSLSensor(*ext_i2c, D4, D5);
  accGyroSensor->init(NULL);

  accGyroSensor->enableAccelerator();
  accGyroSensor->enableGyroscope();
  accGyroSensor->enablePedometer();

  accGyroSensor->setPedometerThreshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  char l[20];
  counter = counter + 1;

  if (digitalRead(USER_BUTTON_B)==LOW) {
      Screen.clean();
      Screen.print(0, "Go away!");
      Screen.print(1, "You're ");
      Screen.print(2, "interupting");
      Screen.print(3, "me!!!");
      do {
          // do nothing 
      }
      while (digitalRead(USER_BUTTON_B)==LOW);
      if (digitalRead(USER_BUTTON_B)==HIGH) {
          Screen.clean();
          Screen.print(0, "Thank you!");  
      }
    }
  if (hasIoTHub && hasWifi && counter == 500) {
    Screen.print(0, "Running...");
    tempSensor->enable();
    tempSensor->getTemperature(&temperature);
    Serial.printf("Temperature %f\n", temperature);
    sprintf(l, "Temp.: %f", temperature); 
    Screen.print(1, l);

    tempSensor->getHumidity(&humidity);
    Serial.printf("Humidity %f\n", humidity);
    sprintf(l, "Humidity: %f", humidity); 
    Screen.print(2, l);

    pressureSensor->getPressure(&pressure);

    char message[128];
    snprintf(message, 128, msg, temperature, humidity, pressure);
    
    Screen.print(3, "Sending to IoT Hub...");
    if (DevKitMQTTClient_SendEvent(message))
    {
      Screen.print(3, "Sent");
    }
    else
    {
      Screen.print(3, "Failure");
    }

    tempSensor->disable();
    tempSensor->reset();
    counter = 0;
  }
  delay(10);

}
