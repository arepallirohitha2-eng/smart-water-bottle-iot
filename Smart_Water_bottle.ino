#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ==============================
// Wi-Fi SETTINGS
// ==============================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ==============================
// PIN DEFINITIONS
// ==============================

#define TEMP_PIN 4
#define BUZZER_PIN 25

#define BUTTON_30 18
#define BUTTON_45 19
#define BUTTON_60 21

#define BATTERY_PIN 34

// ==============================
// TEMPERATURE SENSOR
// ==============================

OneWire oneWire(TEMP_PIN);
DallasTemperature temperatureSensor(&oneWire);

// ==============================
// LCD
// ==============================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==============================
// REMINDER SETTINGS
// ==============================

int reminderMinutes = 45;

unsigned long reminderInterval;
unsigned long previousReminderTime;

// ==============================
// TEMPERATURE LIMITS
// ==============================

// Change these according to your project requirements.
float coldLimit = 10.0;
float hotLimit = 45.0;

// ==============================
// BATTERY SETTINGS
// ==============================

float batteryVoltage = 0.0;
int batteryPercentage = 0;

// ==============================
// FUNCTION DECLARATIONS
// ==============================

void connectWiFi();
void readTemperature();
void checkReminder();
void drinkReminder();
void checkTemperature();
void readBattery();
void checkButtons();
void showNormalDisplay();
void sendNotification(String message);

// ==============================
// SETUP
// ==============================

void setup() {

  Serial.begin(115200);

  // Sensor
  temperatureSensor.begin();

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Buttons
  pinMode(BUTTON_30, INPUT_PULLUP);
  pinMode(BUTTON_45, INPUT_PULLUP);
  pinMode(BUTTON_60, INPUT_PULLUP);

  // Battery
  pinMode(BATTERY_PIN, INPUT);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Water");
  lcd.setCursor(0, 1);
  lcd.print("Bottle");

  delay(2000);

  // Wi-Fi
  connectWiFi();

  // Default reminder = 45 minutes
  reminderInterval = reminderMinutes * 60UL * 1000UL;

  previousReminderTime = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reminder: 45min");

  delay(2000);
}

// ==============================
// MAIN LOOP
// ==============================

void loop() {

  checkButtons();

  readTemperature();

  checkTemperature();

  readBattery();

  checkReminder();

  showNormalDisplay();

  delay(1000);
}

// ==============================
// CONNECT TO WI-FI
// ==============================

void connectWiFi() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {

    delay(500);

    Serial.print(".");

    attempts++;
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println();
    Serial.println("WiFi Connected");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");

  } else {

    Serial.println();
    Serial.println("WiFi Failed");

    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
  }

  delay(1500);
}

// ==============================
// READ TEMPERATURE
// ==============================

void readTemperature() {

  temperatureSensor.requestTemperatures();

  float temperature =
      temperatureSensor.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
}

// ==============================
// TEMPERATURE WARNING
// ==============================

void checkTemperature() {

  float temperature =
      temperatureSensor.getTempCByIndex(0);

  if (temperature == DEVICE_DISCONNECTED_C) {

    Serial.println("Temperature sensor error");

    return;
  }

  // Water is too cold
  if (temperature < coldLimit) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Water Too Cold");

    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print(" C");

    delay(1500);
  }

  // Water is too hot
  else if (temperature > hotLimit) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("WATER TOO HOT!");

    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print(" C");

    // Warning beep
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);

    sendNotification("Warning: Water temperature is high!");

    delay(1500);
  }
}

// ==============================
// CHECK REMINDER
// ==============================

void checkReminder() {

  unsigned long currentTime = millis();

  if (currentTime - previousReminderTime >= reminderInterval) {

    drinkReminder();

    previousReminderTime = currentTime;
  }
}

// ==============================
// DRINK WATER REMINDER
// ==============================

void drinkReminder() {

  Serial.println("Time to drink water!");

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("TIME TO DRINK!");

  lcd.setCursor(0, 1);
  lcd.print("Drink Water");

  // Buzzer
  for (int i = 0; i < 3; i++) {

    digitalWrite(BUZZER_PIN, HIGH);

    delay(400);

    digitalWrite(BUZZER_PIN, LOW);

    delay(300);
  }

  // Mobile notification
  sendNotification("Time to drink water!");

  delay(2000);
}

// ==============================
// BUTTON SELECTION
// ==============================

void checkButtons() {

  // 30 minutes
  if (digitalRead(BUTTON_30) == LOW) {

    reminderMinutes = 30;

    reminderInterval =
        reminderMinutes * 60UL * 1000UL;

    previousReminderTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Reminder Set:");

    lcd.setCursor(0, 1);
    lcd.print("30 Minutes");

    Serial.println("Reminder = 30 minutes");

    delay(1000);
  }

  // 45 minutes
  if (digitalRead(BUTTON_45) == LOW) {

    reminderMinutes = 45;

    reminderInterval =
        reminderMinutes * 60UL * 1000UL;

    previousReminderTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Reminder Set:");

    lcd.setCursor(0, 1);
    lcd.print("45 Minutes");

    Serial.println("Reminder = 45 minutes");

    delay(1000);
  }

  // 60 minutes
  if (digitalRead(BUTTON_60) == LOW) {

    reminderMinutes = 60;

    reminderInterval =
        reminderMinutes * 60UL * 1000UL;

    previousReminderTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Reminder Set:");

    lcd.setCursor(0, 1);
    lcd.print("60 Minutes");

    Serial.println("Reminder = 60 minutes");

    delay(1000);
  }
}

// ==============================
// BATTERY MONITORING
// ==============================

void readBattery() {

  int rawValue = analogRead(BATTERY_PIN);

  // ESP32 ADC reference approximation
  float voltage = (rawValue / 4095.0) * 3.3;

  // Assuming a voltage divider
  batteryVoltage = voltage * 2.0;

  // Convert battery voltage to percentage
  batteryPercentage =
      map(
        batteryVoltage * 100,
        300,
        420,
        0,
        100
      );

  batteryPercentage =
      constrain(batteryPercentage, 0, 100);

  Serial.print("Battery: ");
  Serial.print(batteryVoltage);
  Serial.print(" V | ");

  Serial.print(batteryPercentage);
  Serial.println("%");
}

// ==============================
// LCD NORMAL DISPLAY
// ==============================

void showNormalDisplay() {

  float temperature =
      temperatureSensor.getTempCByIndex(0);

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("Temp:");

  lcd.print(temperature, 1);

  lcd.print("C");

  lcd.setCursor(0, 1);

  lcd.print("R:");

  lcd.print(reminderMinutes);

  lcd.print("m ");

  lcd.print(batteryPercentage);

  lcd.print("%");
}

// ==============================
// MOBILE NOTIFICATION
// ==============================

void sendNotification(String message) {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi not connected");

    return;
  }

  HTTPClient http;

  // Example notification server
  // Replace this URL with your own notification service.

  String serverURL =
      "https://your-notification-server.com/notify";

  http.begin(serverURL);

  http.addHeader(
      "Content-Type",
      "application/json"
  );

  String jsonData =
      "{\"message\":\"" + message + "\"}";

  int httpResponseCode =
      http.POST(jsonData);

  Serial.print("Notification response: ");

  Serial.println(httpResponseCode);

  http.end();
}
