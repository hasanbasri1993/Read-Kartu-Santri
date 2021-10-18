#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Base64.h>
#include <ArduinoJson.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#define OUT_LED 13
#define OUT_BUZZER 12
void setup() {
  pinMode(OUT_LED, OUTPUT);
  pinMode(OUT_BUZZER, OUTPUT);
  digitalWrite(OUT_LED, HIGH);
  digitalWrite(OUT_BUZZER, LOW);
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Initialize Serial Monitor");
  Serial.println("NDEF Reader");
  nfc.begin();
}

void loop() {
  Serial.println("Scan a NFC tag");
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage())
    {
      NdefMessage message = tag.getNdefMessage();
      NdefRecord record = message[0];
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);
      String payloadAsString = "";
      for (int c = 3; c < payloadLength; c++)
        payloadAsString += (char)payload[c];
      const char *inputString = payloadAsString.c_str();
      int inputStringLength = payloadLength - 3;
      int decodedLength = Base64.decodedLength(inputString, inputStringLength);
      char decodedString[decodedLength];
      Base64.decode(decodedString, inputString, inputStringLength);
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, decodedString);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      }else{
        const char *id = doc["id"];
        const char *induk = doc["induk"];
        Serial.println(induk);
        Serial.println(id);
        if (id != "") {
          Serial1.write(induk);
          notif();      
        }
      }
    }
  }
  delay(300);
}

void notif()
{
    tone(OUT_BUZZER, 300);
    digitalWrite(OUT_LED, LOW);
    delay(300);
    noTone(OUT_BUZZER);
    digitalWrite(OUT_LED, HIGH);
}
