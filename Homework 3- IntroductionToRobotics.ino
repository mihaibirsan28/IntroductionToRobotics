const int buttonNoPressed = 0;
const int buttonLongPressed = 1;
const int buttonShortPressed = 2;

const int state1 = 1;
const int state2 = 2;

const byte UP = 0;
const byte DOWN = 1;
const byte LEFT = 2;
const byte RIGHT = 3;

const int minThreshold = 200;
const int maxThreshold = 800;
const int minThresholdSafety = 400;
const int maxThresholdSafety = 600;

const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;

const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

long lastBlinkTime = 0;
long pressingTime = 0;
long lastDebounceTime;
const int debounceDelay = 50;
const int blinkingTime = 500;
const int longPressButton = 2500;

int joystickStateX = 0;
int joystickStateY = 0;
int xValue = 512;
int yValue = 512;
bool joystickMoved = false;

int currentSegment = pinDP;
byte currentSegmentState = LOW;

byte buttonState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;
int buttonDo = buttonNoPressed;

int state = 1;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte segmentsDisplay[] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };

const int nextSegment[segSize][4] = {
  { -1, pinG, pinF, pinB },
  { pinA, pinG, pinF, -1 },
  { pinG, pinD, pinE, pinDP },
  { pinG, -1, pinE, pinC },
  { pinG, pinD, -1, pinC },
  { pinA, pinG, -1, pinB },
  { pinA, pinD, -1, -1 },
  { -1, -1, pinC, -1 }
};


void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
}

int getNextSegment() {

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  joystickStateX = 0;
  joystickStateY = 0;

  if (minThresholdSafety <= xValue && xValue <= maxThresholdSafety && minThresholdSafety <= yValue && yValue <= maxThresholdSafety) {
    joystickMoved = false;
  }

  if (xValue > maxThreshold && joystickMoved == false) {
    joystickStateX = 1;
    joystickMoved = true;
  } else if (xValue < minThreshold && joystickMoved == false) {
    joystickStateX = -1;
    joystickMoved = true;
  }

  if (yValue > maxThreshold && joystickMoved == false) {
    joystickStateY = 1;
    joystickMoved = true;
  } else if (yValue < minThreshold && joystickMoved == false) {
    joystickStateY = -1;
    joystickMoved = true;
  }

  int destionationSegment = currentSegment;
  int currentSegmentForMatrix = currentSegment - 4;

  if (joystickStateX == -1 && nextSegment[currentSegmentForMatrix][DOWN] != -1)
    destionationSegment = nextSegment[currentSegmentForMatrix][DOWN];
  else if (joystickStateX == 1 && nextSegment[currentSegmentForMatrix][UP] != -1)
    destionationSegment = nextSegment[currentSegmentForMatrix][UP];
  if (joystickStateY == -1 && nextSegment[currentSegmentForMatrix][RIGHT] != -1)
    destionationSegment = nextSegment[currentSegmentForMatrix][RIGHT];
  else if (joystickStateY == 1 && nextSegment[currentSegmentForMatrix][LEFT] != -1)
    destionationSegment = nextSegment[currentSegmentForMatrix][LEFT];

  return destionationSegment;
}

void reset7Segment() {
  state = 1;
  for (int i = pinA; i <= pinDP; i++) {
    segmentsDisplay[i] = LOW;
  }
  currentSegment = pinDP;
  buttonDo = buttonNoPressed;
}

int buttonPress() {

  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState != reading) {
      buttonState = reading;
      if (buttonState == LOW)
        pressingTime = millis();
      else if (millis() - pressingTime < longPressButton)
        return buttonShortPressed;

    } else if (buttonState == LOW && millis() - pressingTime >= longPressButton)
        return buttonLongPressed;
  }

  lastReading = reading;
  return buttonNoPressed;
}

void blinkCurrentLed() {
  if (millis() - lastBlinkTime > blinkingTime) {
    lastBlinkTime = millis();
    currentSegmentState = !currentSegmentState;
  }
  digitalWrite(currentSegment, currentSegmentState);
}

void stateAction() {
switch (state) {

    case state1:
      for (int i = 0; i < segSize; i++) {
        if (segments[i] != currentSegment) {
          digitalWrite(segments[i], segmentsDisplay[segments[i]]);
        }
        else {
          digitalWrite(segments[i], LOW);
        }
      }

      blinkCurrentLed();

      buttonDo = buttonPress();

      if (buttonDo == buttonLongPressed) {
        reset7Segment();
      }

      else if (buttonDo == buttonShortPressed) {
        digitalWrite(currentSegment, segmentsDisplay[currentSegment]);
        state = 2;
      }
      currentSegment = getNextSegment();
      break;

    case state2:
      xValue = analogRead(pinX);
      yValue = analogRead(pinY);

      joystickStateX = 0;
      joystickStateY = 0;

      if (minThresholdSafety <= xValue && xValue <= maxThresholdSafety && minThresholdSafety <= yValue && yValue <= maxThresholdSafety) {
        joystickMoved = false;
      }
      if (yValue > maxThreshold && joystickMoved == false) {
        joystickStateY = 1;
        joystickMoved = true;
      } else if (yValue < minThreshold && joystickMoved == false) {
        joystickStateY = -1;
        joystickMoved = true;
      }
      if (joystickStateY == -1) {
        segmentsDisplay[currentSegment] = !segmentsDisplay[currentSegment];

        digitalWrite(currentSegment, segmentsDisplay[currentSegment]);
      }

      buttonDo = buttonPress();
      if (buttonDo == buttonShortPressed) {
        state = 1;
      }
      break;
  }
}

void loop() {
  stateAction();
}