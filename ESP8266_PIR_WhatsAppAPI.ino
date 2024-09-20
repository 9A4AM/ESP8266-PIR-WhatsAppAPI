#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi postavke
const char* ssid = "Tvoj_SSID";
const char* password = "Tvoja_Lozinka";

// CallMeBot API podaci
String phoneNumber = "+385XXXXXXXXX";  // Unesi svoj broj u formatu +385XXXXXXXXX
String apiKey = "Tvoj_API_Kljuc";      // Unesi CallMeBot API ključ

// PIR senzor
const int pirPin = D1;  // Pin na koji je spojen PIR senzor
int motionCount = 0;    // Broji uzastopne detekcije pokreta
unsigned long lastCheckTime = 0;
unsigned long checkInterval = 3000; // 3 sekunde

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);

  // Povezivanje na WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Provjerava PIR senzor svakih 3 sekunde
  if (currentTime - lastCheckTime >= checkInterval) {
    lastCheckTime = currentTime;

    // Čitanje stanja PIR senzora
    int sensorValue = digitalRead(pirPin);

    if (sensorValue == HIGH) {
      motionCount++;
      Serial.println("PIR senzor aktivan!");

      // Ako je pokret detektiran dva puta zaredom
      if (motionCount >= 2) {
        motionCount = 0; // Resetira brojač nakon slanja obavijesti
        sendWhatsAppMessage("PIR senzor je detektirao pokret dvaput zaredom!");
      }
    } else {
      motionCount = 0; // Resetira brojač ako pokret nije detektiran
    }
  }
}

// Funkcija za slanje WhatsApp poruke putem CallMeBot API-ja
void sendWhatsAppMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // CallMeBot API URL
    String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&text=" + message + "&apikey=" + apiKey;

    // Slanje GET zahtjeva
    http.begin(url);
    int httpCode = http.GET();

    // Provjera rezultata
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Poruka poslana! Response:");
      Serial.println(response);
    } else {
      Serial.println("Greška pri slanju poruke.");
    }

    http.end();
  } else {
    Serial.println("WiFi nije povezan.");
  }
}
