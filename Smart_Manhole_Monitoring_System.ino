// Smart Manhole Monitoring System
// BVRIT Hyderabad - ECE Department
// Team: Hansika, Dhatri, Harika, Sanika

#include <SPI.h>
#include <LoRa.h>

// Pin Definitions
#define IR_SENSOR    D1   // Manhole cover detection
#define FLOAT_SENSOR D3   // Water level detection
#define BUZZER       D4   // Local alert buzzer
#define RED_LED      D5   // Alert LED
#define GREEN_LED    D6   // Safe LED

// LoRa Pins
#define SS_PIN    D8
#define RST_PIN   D0
#define DIO0_PIN  D2

void setup() {
  Serial.begin(9600);

  pinMode(IR_SENSOR,    INPUT);
  pinMode(FLOAT_SENSOR, INPUT);
  pinMode(BUZZER,       OUTPUT);
  pinMode(RED_LED,      OUTPUT);
  pinMode(GREEN_LED,    OUTPUT);

  // Initialize LoRa
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("Smart Manhole System Started!");
  Serial.println("Monitoring...");
}

void loop() {
  int coverStatus = digitalRead(IR_SENSOR);
  int waterLevel  = digitalRead(FLOAT_SENSOR);

  // Print status
  Serial.print("Manhole Cover: ");
  Serial.println(coverStatus == LOW ? "OPEN" : "CLOSED");

  Serial.print("Water Level: ");
  Serial.println(waterLevel == HIGH ? "HIGH" : "NORMAL");

  // Check conditions
  if (coverStatus == LOW || waterLevel == HIGH) {
    // ALERT condition
    digitalWrite(BUZZER,    HIGH);
    digitalWrite(RED_LED,   HIGH);
    digitalWrite(GREEN_LED, LOW);

    Serial.println("STATUS: !! ALERT !!");

    // Send LoRa alert
    LoRa.beginPacket();
    if (coverStatus == LOW && waterLevel == HIGH) {
      LoRa.print("ALERT: Cover OPEN & Water HIGH");
      Serial.println("Sending: Cover OPEN & Water HIGH");
    } else if (coverStatus == LOW) {
      LoRa.print("ALERT: Manhole Cover OPEN");
      Serial.println("Sending: Manhole Cover OPEN");
    } else {
      LoRa.print("ALERT: Water Level HIGH");
      Serial.println("Sending: Water Level HIGH");
    }
    LoRa.endPacket();

  } else {
    // SAFE condition
    digitalWrite(BUZZER,    LOW);
    digitalWrite(RED_LED,   LOW);
    digitalWrite(GREEN_LED, HIGH);

    Serial.println("STATUS: SAFE - All Normal");

    // Send LoRa safe status
    LoRa.beginPacket();
    LoRa.print("STATUS: Normal - Cover Closed & Water Normal");
    LoRa.endPacket();
  }

  Serial.println("----------------------------");
  delay(2000);
}