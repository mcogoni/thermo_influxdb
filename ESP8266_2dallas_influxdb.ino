    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>
    #include <InfluxDb.h>
    #include <DallasTemperature.h>

    // Data wire is plugged into pin D3 on the ESP8266
    #define ONE_WIRE_BUS D3
    
    #define TEMP_SIZE 7

    
    #define INFLUXDB_HOST "192.168.1.89:8089"   //Enter IP of device running Influx Database
    #define WIFI_SSID "ssid"              //Enter SSID of your WIFI Access Point
    #define WIFI_PASS "password"          //Enter Password of your WIFI Access Point
    ESP8266WiFiMulti WiFiMulti;
    Influxdb influx(INFLUXDB_HOST);

    // Setup a oneWire instance to communicate with any OneWire devices
    OneWire oneWire(ONE_WIRE_BUS);
    
    // Pass our oneWire reference to Dallas Temperature.
    DallasTemperature DS18B20(&oneWire);

    float temp_IN, temp_OUT;
    
    void setup() {
      Serial.begin(9600);
      WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
      Serial.print("Connecting to WIFI");
      while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
      }
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      influx.setDb("influxdb");
      Serial.println("Setup Complete.");
      delay(2000);

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

      InfluxData row("data");
      row.addTag("Device", "ESP8266_thermo");
      row.addTag("Sensor", "FloorTemp");
      row.addTag("Unit", "Celsius");
      row.addValue("TempIN", temp_IN);
      row.addValue("TempOUT", temp_OUT);
      influx.write(row);
      delay(30000);
      Serial.println("-");
      Serial.println(temp_IN);
      Serial.println(temp_OUT);
      
    }
