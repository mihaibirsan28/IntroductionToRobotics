const bool buttonPressedState = 1;
const bool buttonNotPressedState = 0;

const int noPedestriansCrossing = -1;

const int carCrossingState = 0;
const int yellowLightState = 1;
const int pedestrianCrossingState = 2;
const int greenPedestrianLightBlinkingState = 3;

const int carCrossingTime = 8000;
const int yellowLightTime = 3000;
const int pedestrianCrossingTime = 8000;
const int greenPedestrianLightBlinkingTime = 4000;

const int buzzerPin = 2;
const int buttonPin = 3;

const int trafficRedLed = 4;
const int trafficYellowLed = 5;
const int trafficGreenLed = 6;

const int pedestrianRedLed = 7;
const int pedestrianGreenLed = 8;

int globalState;
byte buttonState = HIGH;
int lastReading;
int currentTime = 0;
int lastChangeTime = noPedestriansCrossing;

int lastDebounceTime = 0;
int debounceDelay = 50;

long lastPedestrianLightChangeTime = 0;
long pedestrianLightBlinkingInterval = 500;
byte pedestrianGreenLightState = LOW;

int buzzerState = LOW;
int lastBuzzerChangeTime = 0;
int buzzerCrossingTone = 500;
int buzzerBlinkingTone = 400;

int buzzerCrossingInterval = 800;
int buzzerBlinkingInterval = 400;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  pinMode(trafficRedLed, OUTPUT);
  pinMode(trafficYellowLed, OUTPUT);
  pinMode(trafficGreenLed, OUTPUT);

  pinMode(pedestrianRedLed, OUTPUT);
  pinMode(pedestrianGreenLed, OUTPUT);

  globalState = carCrossingState;
  changeLights(LOW, LOW, HIGH, HIGH, LOW);
  Serial.begin(9600);
}

byte getButtonState() {
  byte reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading)
      buttonState = reading;
  }

  lastReading = reading;

  return buttonState;
}

void changeLights(bool trafficRed, bool trafficYellow, bool trafficGreen, bool pedestrianRed, bool pedestrianGreen) {
  digitalWrite(trafficRedLed, trafficRed);
  digitalWrite(trafficYellowLed, trafficYellow);
  digitalWrite(trafficGreenLed, trafficGreen);

  digitalWrite(pedestrianRedLed, pedestrianRed);
  digitalWrite(pedestrianGreenLed, pedestrianGreen);
}


void buzzerSound(int buzzerTone, int buzzerInterval) {
  if ((currentTime - lastBuzzerChangeTime) > buzzerInterval) {
    lastBuzzerChangeTime = currentTime;
    buzzerState = !buzzerState;
  }
  if (buzzerState == LOW) {
    noTone(buzzerPin);
  } 
  else {
    tone(buzzerPin, buzzerTone);
  }
}

void stateAction() {
  int buttonValue = getButtonState();
  currentTime = millis();

  if (globalState == carCrossingState && lastChangeTime == noPedestriansCrossing && buttonValue == buttonNotPressedState) {
    lastChangeTime = currentTime;
    buzzerState = LOW;
  }

  if (lastChangeTime != noPedestriansCrossing) {
    if (globalState == carCrossingState) {
      if (currentTime - lastChangeTime > carCrossingTime) {
        globalState = yellowLightState;
        lastChangeTime = currentTime;
      }
      changeLights(LOW, LOW, HIGH, HIGH, LOW);
    }

    if (globalState == yellowLightState) {
      if (currentTime - lastChangeTime > yellowLightTime) {
        globalState = pedestrianCrossingState;
        lastChangeTime = currentTime;
      }

      changeLights(LOW, HIGH, LOW, HIGH, LOW);
    }

    if (globalState == pedestrianCrossingState) {
      if (currentTime - lastChangeTime > pedestrianCrossingTime) {
        globalState = greenPedestrianLightBlinkingState;
        lastChangeTime = currentTime;
      }

      changeLights(HIGH, LOW, LOW, LOW, HIGH);
      buzzerSound(buzzerCrossingTone, buzzerCrossingInterval);
    }

    if (globalState == greenPedestrianLightBlinkingState) {
      if (currentTime - lastChangeTime > greenPedestrianLightBlinkingTime) {
        globalState = carCrossingState;
        lastChangeTime = noPedestriansCrossing;
        buzzerState = LOW;
      }
      buzzerSound(buzzerBlinkingTone, buzzerBlinkingInterval);

      if ((millis() - lastPedestrianLightChangeTime) > pedestrianLightBlinkingInterval) {
        lastPedestrianLightChangeTime = currentTime;
        pedestrianGreenLightState = !pedestrianGreenLightState;
      }

      changeLights(HIGH, LOW, LOW, LOW, pedestrianGreenLightState);
    }
  } 
  else {
    changeLights(LOW, LOW, HIGH, HIGH, LOW);
  }
}

void loop() {
  stateAction();
}