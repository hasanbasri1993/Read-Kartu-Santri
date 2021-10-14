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
void setup(void)
{
  pinMode(OUT_LED, OUTPUT);
  pinMode(OUT_BUZZER, OUTPUT);
  digitalWrite(OUT_LED, HIGH);
  digitalWrite(OUT_BUZZER, LOW);
  Serial.begin(115200); //This pipes to the serial monitor
  Serial1.begin(115200);
  Serial.println("Initialize Serial Monitor");
  Serial.println("NDEF Reader");
  nfc.begin();
}

void loop(void)
{
  Serial.println("Scan a NFC tag");
  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");
    Serial.println(tag.getUidString());

    if (tag.hasNdefMessage()) // every tag won't have a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis NFC Tag contains an NDEF Message with ");
      Serial.print(message.getRecordCount());
      Serial.print(" NDEF Record");
      if (message.getRecordCount() != 1)
      {
        Serial.print("s");
      }
      Serial.println(".");
      Serial.println("\nNDEF Record ");
      NdefRecord record = message[0]; // alternate syntax
      
      //Serial.print("  TNF: ");Serial.println(record.getTnf());
      //Serial.print("  Type: ");  Serial.println(record.getType()); // will be "" for TNF_EMPTY
      
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);
      String payloadAsString = "";
      for (int c = 3; c < payloadLength; c++)
        payloadAsString += (char)payload[c];
      //Serial.print("  Payload (as String): ");Serial.println(payloadAsString);
      const char *inputString = payloadAsString.c_str();
      int inputStringLength = payloadLength - 3;
      //Serial.print("Input string is:");Serial.println(inputString);
      int decodedLength = Base64.decodedLength(inputString, inputStringLength);
      char decodedString[decodedLength];
      Base64.decode(decodedString, inputString, inputStringLength);
      //Serial.print("Decoded string is:\t");Serial.println(decodedString);
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, decodedString);
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      const char *id = doc["id"];       // "6447"
      const char *induk = doc["induk"]; // "206085"
      Serial.println(induk);
      Serial.println(id);
      if (id != "")
      {
        Serial1.write(induk);
        digitalWrite(OUT_BUZZER, HIGH);
        digitalWrite(OUT_LED, LOW);
        delay(500);
        digitalWrite(OUT_BUZZER, LOW);
        digitalWrite(OUT_LED, HIGH);
      }

      // id is probably blank and will return ""
      String uid = record.getId();
      if (uid != "")
      {
        Serial.print("  ID: ");
        Serial.println(uid);
      }
    }
  }
  delay(500);
}
