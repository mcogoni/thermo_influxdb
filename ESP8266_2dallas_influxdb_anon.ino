    #include <ESP8266WiFi.h>
    #include <InfluxDb.h>
    #include <DallasTemperature.h>

    #include <Wire.h>
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BME280.h>
        
    // Data wire is plugged into pin D3 on the ESP8266
    #define ONE_WIRE_BUS D3
    
    #define INFLUXDB_HOST "192.168.1.89:8089"   //Enter IP of device running Influx Database
    #define WIFI_SSID "ssid"              //Enter SSID of your WIFI Access Point
    #define WIFI_PASS "password"          //Enter Password of your WIFI Access Point

    Influxdb influx(INFLUXDB_HOST);

    // Setup a oneWire instance to communicate with any OneWire devices
    OneWire oneWire(ONE_WIRE_BUS);
    
    DallasTemperature DS18B20(&oneWire);
    float temp_IN, temp_OUT,temp_HOME, press_HOME, hum_HOME;

    Adafruit_BME280 bme; // I2C

    void init_bme280() {
      Serial.println(F("BME280 test"));
    
      bool status;
    
      // default settings
      // (you can also pass in a Wire library object like &Wire2)
      status = bme.begin(0x76);  
      if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
      }
    
      Serial.println("-- Default Test --");
    
      Serial.println();
    }


    void init_wifi() {
      // We start by connecting to a WiFi network
      
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(WIFI_SSID);
      
      /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
         would try to act as both a client and an access-point and could cause
         network-issues with your other WiFi-devices on your WiFi-network. */
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }

    void setup() {
      Serial.begin(115200);
      init_wifi();

      init_bme280();
      
      influx.setDb("influxdb");
      Serial.println("InfluxDB setup complete.");
      delay(1000);

      DS18B20.begin();
      Serial.println("DALLAS sensors setup complete.");

    }

    float getTemperature(int index) {

      float temp;
      do {
             DS18B20.requestTemperatures();
             temp = DS18B20.getTempCByIndex(index);
          } while (temp == 85.0 || temp == (-127.0));
      //Serial.println(temp);
      return temp;
    }


    void loop() {

      temp_IN = getTemperature(1);
      temp_OUT = getTemperature(0);
      temp_HOME = bme.readTemperature();
      press_HOME = bme.readPressure() / 100.0F;
      hum_HOME = bme.readHumidity();

      InfluxData row("data");
      row.addTag("Device", "ESP8266_thermo");
      row.addTag("Sensor", "FloorTemp");
      row.addTag("Unit", "Celsius");
      row.addValue("TempIN", temp_IN);
      row.addValue("TempOUT", temp_OUT);
      row.addValue("TempHome", temp_HOME);
      row.addValue("PressHome", press_HOME);
      influx.write(row);
      delay(30000);
      Serial.println("-");
      Serial.println(temp_IN);
      Serial.println(temp_OUT);
      Serial.println(temp_HOME);
      Serial.println(press_HOME);
      Serial.println(hum_HOME);

      if (WiFi.status() != WL_CONNECTED) {
        init_wifi();
        Serial.println("WIFI not connected!");
      }
    }
