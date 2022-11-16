const int buttonNoPressed = 0;
const int buttonLongPressed = 1;
const int buttonShortPressed = 2;

const int state1 = 1;
const int state2 = 2;

const byte UP = 1;
const byte DOWN = -1;
const byte LEFT = -1;
const byte RIGHT = 1;

const int minThreshold = 200;
const int maxThreshold = 800;
const int minThresholdSafety = 400;
const int maxThresholdSafety = 600;

const int latchPin = 11;  // STCP to 12 on Shift Register
const int clockPin = 10;  // SHCP to 11 on Shift Register
const int dataPin = 12;   // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int regSize = 8;

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

int currentDisplay = 0;
byte currentDisplayState = LOW;

byte buttonState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;
int buttonDo = buttonNoPressed;

int state = 1;

const int encodingsNumber = 16;

int digitArray[16] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B11111110,  // B
  B10011100,  // C
  B11111100,  // D
  B10011110,  // E
  B10001110   // F
};

byte dpLedState[] = { 0, 0, 0, 0 };

int displayValue[] = { 0, 0, 0, 0 };

byte displayDigits[] = { segD1, segD2, segD3, segD4 };

const int numberOfDisplays = 4;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);  //
  pinMode(dataPin, OUTPUT);   //ds

  for (int i = 0; i < numberOfDisplays; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}

void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {
  for (auto display : displayDigits) {
    digitalWrite(display, HIGH);
  }
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeAllDigits() {
  for (int i = 0; i < numberOfDisplays; i++) {
    writeReg(digitArray[displayValue[i]] ^ dpLedState[i]);
    activateDisplay(i);
    delay(3);
  }
  digitalWrite(displayDigits[numberOfDisplays - 1], HIGH);
}

int getNextDisplay() {

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

  int destinationDisplay = currentDisplay;

  if (joystickStateX == RIGHT) {
    if (currentDisplay + 1 < numberOfDisplays)
      destinationDisplay = currentDisplay + 1;
    else
      destinationDisplay = 0;
    ;
  }
  if (joystickStateX < 0) {
    if (currentDisplay - 1 >= 0)
      destinationDisplay = currentDisplay - 1;
    else
      destinationDisplay = numberOfDisplays - 1;
  }

  if (destinationDisplay != currentDisplay)
    dpLedState[currentDisplay] = 0;

  return destinationDisplay;
}

void resetDisplays() {
  state = 1;
  for (int i = 0; i < numberOfDisplays; i++) {
    displayValue[i] = 0;
    dpLedState[i] = 0;
  }
  currentDisplay = 0;
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

void changeDigit(int direction) {
  if (direction == UP) {
    if (displayValue[currentDisplay] + 1 <= encodingsNumber - 1)
      displayValue[currentDisplay]++;
    else
      displayValue[currentDisplay] = 0;
  } else {
    if (displayValue[currentDisplay] - 1 >= 0)
      displayValue[currentDisplay]--;
    else
      displayValue[currentDisplay] = encodingsNumber - 1;
  }
}

void stateAction() {

  writeAllDigits();

  switch (state) {

    case state1:
      if (millis() - lastBlinkTime > blinkingTime) {
        lastBlinkTime = millis();
        dpLedState[currentDisplay] = !dpLedState[currentDisplay];
      }
      buttonDo = buttonPress();
      if (buttonDo == buttonLongPressed)
        resetDisplays();

      else if (buttonDo == buttonShortPressed)
        state = 2;

      else
        currentDisplay = getNextDisplay();

      break;

    case state2:
      dpLedState[currentDisplay] = 1;
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
      if (joystickStateY != 0) {
        changeDigit(joystickStateY);
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