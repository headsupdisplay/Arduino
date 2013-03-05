
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;


void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop()
{
  sweepUpDown(redPin, bluePin, 1);
  sweepUpDown(greenPin, redPin, 1);
  sweepUpDown(bluePin, greenPin, 1);
  sweepUpDownAll(redPin, greenPin, bluePin, 1);
}

void sweepUpDown(int pin1, int pin2, int wait)
{
  for(int i = 0; i < 256; i++)
  {
    analogWrite(pin1, i);
    analogWrite(pin2, i);
    delay(wait);
  }
  for(int i = 255; i >= 0; i--)
  {
    analogWrite(pin1, i);
    analogWrite(pin2, i);
    delay(wait);
  }
}

void sweepUpDownAll(int pinR, int pinG, int pinB, int wait)
{
  for(int i = 0; i < 256; i++)
  {
    analogWrite(pinR, i);
    analogWrite(pinG, i);
    analogWrite(pinB, i);
    delay(wait);
  }
  for(int i = 255; i >= 0; i--)
  {
    analogWrite(pinR, i);
    analogWrite(pinG, i);
    analogWrite(pinB, i);
    delay(wait);
  }
}
