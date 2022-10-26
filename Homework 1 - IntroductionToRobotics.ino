const int potPinRed = A0;
const int potPinGreen = A1;
const int potPinBlue = A2;

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;

int redPinValue = 0;
int greenPinValue = 0;
int bluePinValue = 0;

const float minBrightness = 0.0;
const float maxBrightness = 255.0;
const float minAnalogValue = 0.0;
const float maxAnalogValue = 1023.0;

void setup() {
  pinMode(potPinRed, INPUT);
  pinMode(potPinBlue, INPUT);
  pinMode(potPinGreen, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void loop() {
  redPotValue = analogRead(potPinRed);
  bluePotValue = analogRead(potPinBlue);
  greenPotValue = analogRead(potPinGreen);

  redPinValue = map(redPotValue, minAnalogValue, maxAnalogValue, minBrightness, maxBrightness);
  bluePinValue = map(bluePotValue, minAnalogValue, maxAnalogValue, minBrightness, maxBrightness);
  greenPinValue = map(greenPotValue, minAnalogValue, maxAnalogValue, minBrightness, maxBrightness);

  analogWrite(redPin, redPinValue);
  analogWrite(bluePin, bluePinValue);
  analogWrite(greenPin, greenPinValue);
}
