#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
const char* authToken = "your bearer token";
const char* ssid = "M20_Pro";
const char* password = "hazem123";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    .button {
      display: inline-block;
      padding: 10px 20px;
      margin-right: 10px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
      text-align: center;
      text-decoration: none;
      font-size: 16px;
    }
  </style>
</head><body>
  <p>search your dog by espId</p>
  <form action="/on">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <a href="/off"><button class="button">add esp codes for existing dogs</button></a>
</body></html>)rawliteral";


const char indext_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    .button {
      display: inline-block;
      padding: 10px 20px;
      margin-right: 10px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
      text-align: center;
      text-decoration: none;
      font-size: 16px;
    }
  </style>
</head><body>
  <p>ajouter un esp id pour un chien existant</p>
    <form action="/get">
    input2: <input type="text" name="input2">
      input3: <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form><br>
  <br><a href=\"/\">Return to Home Page</a>
</body></html>)rawliteral";


const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";
void setup() {
    Serial.begin(115200);
    delay(4000);
    WiFi.mode(WIFI_STA);  
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi..");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to the WiFi network");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
       
        request->send_P(200, "text/html", index_html);
    });


      server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
       
        request->send_P(200, "text/html", indext_html);
    });
   
   
      server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputMessage1; 
        String inputMessage2;
        if (request->hasParam(PARAM_INPUT_2) &&  request->hasParam(PARAM_INPUT_3)) {
      inputMessage1 = request->getParam(PARAM_INPUT_2)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_3)->value();
     String message = "http://192.168.193.140:8080/dogs/addEsp/";
 message += inputMessage1;
 message+="/";
 message += inputMessage2;

        HTTPClient http;
            
            http.begin(message);
  http.addHeader("Authorization", authToken);
    int httpCode = http.sendRequest("PUT");
 
            if (httpCode > 0) {
                Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                if (httpCode == HTTP_CODE_OK) {
                  
                    
                    
                     request->send(200, "text/html", "le chien avec id  " + inputMessage1 +"  a recu l'id :"+inputMessage2+
                                     "  avec succes <br><a href=\"/\">Return to Home Page</a>"); 
                }
                else {
                     request->send(200, "text/html", "verifier vos parametres  <br><a href=\"/\">Retourner a la page home </a>"); 
                }}
      
    }
       
         else {
      inputMessage1 = "coordaonneees manquants";
     
    }

        
    });


    server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputMessage;
        String inputParam;
        
        // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
        if (request->hasParam(PARAM_INPUT_1)) {
            inputMessage = request->getParam(PARAM_INPUT_1)->value();
            inputParam = PARAM_INPUT_1;
            String message = "http://192.168.193.140:8080/dogs/geEsp/";
            message += inputMessage;

            Serial.println(message);
            HTTPClient http;
            
            http.begin(message);
  http.addHeader("Authorization", authToken);
          int httpCode = http.GET();
 
            if (httpCode > 0) {
                Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                if (httpCode == HTTP_CODE_OK) {
                    String payload = http.getString();
                    JSONVar myObject = JSON.parse(payload);
                    Serial.print("the json is ");
                    Serial.println(myObject["id"]);
              String responseHtml = "<!DOCTYPE html><html><head><title></title></head><body>";

                  
                  String s =( myObject["imageUrl"]);
responseHtml += "<p style='text-align: center;'><strong></strong> <img src='" + s + "' style='max-width: 300px;' /></p>";

responseHtml += "<p style='text-align: center;'><strong style='color: red;'>Race:</strong> " + String(JSON.stringify(myObject["race"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>Birthdate:</strong> " + String(JSON.stringify(myObject["birthdate"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>Sex:</strong> " + String(JSON.stringify(myObject["sexe"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>vaccination rage:</strong> " + String(JSON.stringify(myObject["vaccinated"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>Nom proprietaire: </strong> " + String(JSON.stringify(myObject["nameProp"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>numero proprietaire:</strong> " + String(JSON.stringify(myObject["phoneProp"])) + "</p>";
responseHtml += "<p style='text-align: center;'><strong style='color: red;'>mail proprietaire: </strong> " + String(JSON.stringify(myObject["mailProp"])) + "</p>";




responseHtml += "<br><a href='/'>Retour a la page d'accueil</a></body></html>";

request->send(200, "text/html", responseHtml);

                }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
            http.end();
        } else {
            request->send(200, "text/html", "<p>please submit the id of your dog</p>" \
                                             "<br><a href=\"/\">Return to Home Page</a>");
        }
    });

    server.onNotFound(notFound);
    server.begin();
}

void loop() {

}


