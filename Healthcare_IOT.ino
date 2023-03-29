
#include "DHT.h"
#define DHTTYPE DHT11
const int DHTPin = D5;
DHT dht(DHTPin, DHTTYPE); //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);

#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;
uint32_t tsLastReport = 0;

#define DS18B20 D4
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
/*Put your SSID & Password*/
const char* ssid = "my wifi";  // Enter SSID here
const char* password = "123456788";  //Enter Password here

float temperature, humidity, BPM, SpO2, bodytemperature, t, h;
int coi = D8;



void setup() {
  Serial.begin(9600);

  //---------------------------------------
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  //---------------------------------------
    dht.readTemperature();
    dht.readHumidity();
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  
      Serial.println("SSD1306 allocation failed");
    }
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);          
    display.setCursor(0,40);             
    display.println("DUC TUE");
    display.display();
    display.setTextSize(1);
    pinMode(coi,OUTPUT);


  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
  } else {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    dht.readTemperature();
    dht.readHumidity();

}
//---------------------------------------
void onBeatDetected() {
  Serial.println("Beat!");
}
void loop() {
    pox.update();
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
        display.clearDisplay();     
        display.setCursor(0,0); 
        display.print("HeartR: ");
        display.setCursor(60,0);
        display.print(BPM); 
        display.println(" bpm");
        
        display.setCursor(0,12);
        display.print("SpO2: ");
        display.setCursor(60,12);
        display.print(SpO2);
        display.println(" %");
    float t = dht.readTemperature();
    String Temperature_Value = String(t);
    float h = dht.readHumidity();
    String Humidity_Value = String(h);
    temperature = t;
    humidity = h;
    sensors.requestTemperatures();
    bodytemperature = sensors.getTempCByIndex(0);

        
        display.setCursor(0,24); 
        display.print("TempRoom:");
        display.setCursor(60,24);
        display.print(t); 
        display.println(" 'C");
        
        display.setCursor(0,36);
        display.print("Humidity:");
        display.setCursor(60,36);
        display.print(h);
        display.println(" %");
        
        display.setCursor(0,48); 
        display.print("TempBody:");
        display.setCursor(60,48);
        display.print(bodytemperature); 
        display.println(" 'C");
        
        display.display();
  if (millis() - tsLastReport > 1000)
  {
    Serial.print("BPM: ");
    Serial.println(pox.getHeartRate());
    Serial.print("SpO2:  ");
    Serial.print(pox.getSpO2());
    Serial.println("%");

   
    Serial.print("Room Temperature: ");
    Serial.print(Temperature_Value);
    Serial.println("°C");

    Serial.print("Room Humidity: ");
    Serial.print(Humidity_Value);
    Serial.println("%");

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println("°C");
    Serial.println("*********************************");
    if((bodytemperature>39)&&(humidity<50)) {
      digitalWrite(coi, HIGH);
    }
    tsLastReport = millis();
  }
    server.handleClient();

}

void handle_OnConnect() {
    server.send(200, "text/html", SendHTML(BPM, SpO2, temperature, humidity,bodytemperature));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float BPM, float SpO2, float temperature, float humidity, float bodytemperature) {
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<link rel='stylesheet' href='https://unicons.iconscout.com/release/v4.0.0/css/line.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += ".box-full{";
  ptr += "width: 600px !important;";
  ptr += "margin-left: 25% !important;";
  ptr += "}";
  ptr += "</style>";

  //Ajax Code Start
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,1000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  //Ajax Code END

  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>MAX30100 ESP8266 WebServer</h1>";
  ptr += "</div>";
  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Sensor Readings</h2>";
  ptr += "<div class='sensors-container'>";

  //For Heart Rate
  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-heartbeat' style='color:#cc3300'></i>";
  ptr += "<span class='sensor-labels'> Nhịp tim </span>";
  ptr += (float)BPM;
  ptr += "<sup class='units'>BPM</sup>";
  ptr += "</p>";
  ptr += "<hr>";

  //For Sp02
  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-burn' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> Nồng độ oxy trong máu </span>";
  ptr += (float)SpO2;
  ptr += "<sup class='units'>%</sup>";
  ptr += "</p>";
  ptr += "<hr>";
  
  //For temperature room
  ptr += "<p class='sensor'>";
  ptr += "<i class='uil uil-brightness' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> Nhiệt độ môi trường </span>";
  ptr += (float)temperature ;
  ptr += "<sup class='units'>°C</sup>";
  ptr += "</p>";
  ptr += "<hr>";

  //For humidity
  ptr += "<p class='sensor'>";
  ptr += "<i class='uil uil-water' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> Độ ẩm không khí </span>";
  ptr += (float)humidity;
  ptr += "<sup class='units'>%</sup>";
  ptr += "</p>";

  //For bodytemperature
  ptr += "<p class='sensor'>";
  ptr += "<i class='uil uil-temperature-three-quarter' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> Nhiệt độ cơ thể </span>";
  ptr += (float)bodytemperature ;
  ptr += "<sup class='units'>°C</sup>";
  ptr += "</p>";
  ptr += "<hr>";

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</body>";
  ptr += "</html>";
  return ptr;
}
