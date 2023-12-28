#include "Pins.h"
#include "LCD.h"
#include "Concurrent.h"

void setup()
{
  pinMode(MOISTURE_SENSOR_POWER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PUMP_STATUS_PIN, OUTPUT);
  pinMode(LED_WARNING_STATUS_PIN, OUTPUT);
  pinMode(LED_OK_STATUS_PIN, OUTPUT);

  digitalWrite(MOISTURE_SENSOR_POWER_PIN, LOW);

  // Use HIGH to turn off since Digital Pins are active low
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PUMP_STATUS_PIN, HIGH);
  digitalWrite(LED_WARNING_STATUS_PIN, HIGH);
  digitalWrite(LED_OK_STATUS_PIN, HIGH);

  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Print a message to the LCD.
  lcd.setCursor(4, 0);
  lcd.print("MOISTURE");
  lcd.setCursor(3, 1);
  lcd.print("WATCHER V1");
  delay(2000);

  lcd.clear();
}

void loop()
{
  getMoistureData();
}

void getMoistureData()
{
  int moisture = sensorData();
  int moisturePercentage = 100 - (moisture / 10);

  Serial.println("SOIL MOISTURE: " + String(moisture) + " | " + String(moisturePercentage) + "%");

  moistureStatus(moisturePercentage);
  pumpRunner(moisturePercentage);
  ledStatus(moisturePercentage);
}

int sensorData() {
  digitalWrite(MOISTURE_SENSOR_POWER_PIN, HIGH);  // Turn the sensor ON
  delay(50);                                      // Allow power to settle

  int moisture = analogRead(MOISTURE_SENSOR_PIN); // Read the analog value form sensor

  digitalWrite(MOISTURE_SENSOR_POWER_PIN, LOW);   // Turn the sensor OFF

  return moisture;
}

void moistureStatus(int moisturePercentage) {
  concurrent(200, [moisturePercentage]() {
    lcd.setCursor(0, 0);
    lcd.print("MOISTURE: ");
    lcd.setCursor(10, 0);
    lcd.print("    ");
    lcd.setCursor(10, 0);

    // Display the moisture value using a percentage.
    // The smaller the moisture value, the wetter the soil.
    // If the moisture value is 0, then display 100%, vice versa.
    lcd.print(String(moisturePercentage) + "%");
  });
}

void pumpRunner(int moisturePercentage) {
  concurrent(200, [moisturePercentage]() {
    if (moisturePercentage <= 50) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PUMP_STATUS_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.print("WATER PUMP: ON ");
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PUMP_STATUS_PIN, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("WATER PUMP: OFF");
    }
  });
}

void ledStatus(int moisturePercentage) {
  concurrent(50, [moisturePercentage]() {
    if (moisturePercentage <= 60) {
      digitalWrite(LED_OK_STATUS_PIN, HIGH);
      digitalWrite(LED_WARNING_STATUS_PIN, LOW);
    } else {
      digitalWrite(LED_OK_STATUS_PIN, LOW);
      digitalWrite(LED_WARNING_STATUS_PIN, HIGH);
    }
  });
}
