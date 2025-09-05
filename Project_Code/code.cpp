const int mq2Pin       = A0;  // MQ-2 analog input
const int flameD0Pin   = 2;   // Flame sensor digital output (LOW = flame)
const int buzzerPin    = 13;
const int blueLedPin   = 6;
const int redLedPin    = 8;

// Relay control pins (active LOW)
const int relayValvePin = 11; // Solenoid Valve
const int relayFanPin   = 10; // Fan
const int relayMotorPin = 5;  // Water Pump

// Thresholds
const int gasTriggerThreshold = 300;  // trigger gas alert
const int gasResetThreshold   = 200;  // allow reset after dropping below this

// Hold times
const unsigned long gasHoldTime   = 2000; // 2 sec
const unsigned long flameHoldTime = 5000; // 5 sec

// Timing variables
unsigned long gasLastDetect   = 0;
unsigned long flameLastDetect = 0;
bool gasActive   = false;
bool flameActive = false;

void setup() {
  pinMode(flameD0Pin, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  pinMode(relayValvePin, OUTPUT);
  pinMode(relayFanPin, OUTPUT);
  pinMode(relayMotorPin, OUTPUT);

  resetSystem();  // start safe

  Serial.begin(9600);
}

void loop() {
  int gasValue = analogRead(mq2Pin);
  int flameState = digitalRead(flameD0Pin);

  Serial.print("MQ-2: ");
  Serial.print(gasValue);
  Serial.print(" | Flame D0: ");
  Serial.println(flameState == LOW ? "FLAME" : "NONE");

  // --------- GAS DETECTION ----------
  if (gasValue > gasTriggerThreshold) {
    triggerGasAlert();
    gasLastDetect = millis();
    gasActive = true;
  }

  // --------- FLAME DETECTION ----------
  if (flameState == LOW) {
    triggerFlameAlert();
    flameLastDetect = millis();
    flameActive = true;
  }

  // --------- CHECK TIMERS ----------
  if (gasActive && millis() - gasLastDetect >= gasHoldTime && gasValue < gasResetThreshold) {
    gasActive = false;
  }
  if (flameActive && millis() - flameLastDetect >= flameHoldTime) {
    flameActive = false;
  }

  // If no alert active → reset
  if (!gasActive && !flameActive) {
    resetSystem();
  }

  delay(100);
}

// ---------------- FUNCTIONS ----------------
void triggerGasAlert() {
  digitalWrite(redLedPin, HIGH);
  digitalWrite(blueLedPin, LOW);
  digitalWrite(buzzerPin, LOW);

  digitalWrite(relayValvePin, LOW);  // ON
  digitalWrite(relayFanPin,   LOW);  // ON
  digitalWrite(relayMotorPin, HIGH); // Pump OFF

  Serial.println("🚨 GAS ALERT");
}

void triggerFlameAlert() {
  digitalWrite(redLedPin, HIGH);
  digitalWrite(blueLedPin, LOW);
  digitalWrite(buzzerPin, LOW);

  digitalWrite(relayValvePin, LOW);  // ON
  digitalWrite(relayFanPin,   LOW);  // ON
  digitalWrite(relayMotorPin, LOW);  // Pump ON

  Serial.println("🔥 FLAME ALERT");
}

void resetSystem() {
  digitalWrite(redLedPin, LOW);
  digitalWrite(blueLedPin, HIGH);
  digitalWrite(buzzerPin, HIGH);

  digitalWrite(relayValvePin, HIGH);
  digitalWrite(relayFanPin,   HIGH);
  digitalWrite(relayMotorPin, HIGH);

  Serial.println("✅ System back to normal");
}