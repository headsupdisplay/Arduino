#define OFF 0
#define ON 1
#define COLOR 2
#define BRIGHT 3
#define BLINK 4
#define BLINK_COLOR 5
#define BLINK_BRIGHT 6
#define SENSOR 7
#define PUSH_HIGH 8
#define PUSH_LOW 9
#define PUSH_ANALOG 10
#define GET_DIGITAL 11
#define GET_ANALOG 12
#define FIVE_MINUTE_RED_TO_GREEN 13

#define RED 0
#define GREEN 1
#define BLUE 2

#define BLINK_OFF_MS 0
#define BLINK_ON_MS 1

#define NOT_BLINKING 0
#define BLINKING_AND_ON 1
#define BLINKING_AND_OFF 2

#define NUM_MONO_LEDS 3
#define MONO_LED_PINS 9, 10, 11

#define NUM_COLOR_LEDS 1
#define COLOR_LED_PINS 3, 5, 6

#define SECONDS_ON 8
#define GREEN_OFF 60
#define YELLOW_OFF 180
#define RED_OFF 240
#define BLINK_START 270

#line 1 "Serial_HW_Comm.ino"
#include <SoftwareSerial.h>
#include <MinHudOpcodes.h>

#include "Arduino.h"
void setup();
void loop();
byte* doOperation(byte operation[]);

SoftwareSerial btSerial(8, 9); // RX, TX
boolean readingOperation;
byte* operationIn;
int numBytesRead;
boolean fiveMinuteTimerOn;
long fiveMinuteTimerMs;

//User peripheral values
byte monoPins[] = {MONO_LED_PINS};
byte colorPins[NUM_COLOR_LEDS][3];

byte brightness[NUM_MONO_LEDS];
byte color[NUM_COLOR_LEDS][3];

byte blink_brightness[NUM_MONO_LEDS];
byte blink_color[NUM_COLOR_LEDS][3];

byte redValues[SECONDS_ON];
byte greenValues[SECONDS_ON];

unsigned short blink_off[NUM_MONO_LEDS + NUM_COLOR_LEDS];
unsigned short blink_on[NUM_MONO_LEDS + NUM_COLOR_LEDS];
byte blink_state[NUM_MONO_LEDS + NUM_COLOR_LEDS];
long unsigned lastBlinkTransitionTime[NUM_MONO_LEDS + NUM_COLOR_LEDS];

void initFiveMinuteTimer()
{
  fiveMinuteTimerOn = true;
  fiveMinuteTimerMs = millis();
  blink_state[3] = NOT_BLINKING;
  blink_on[3] = 50;
  blink_off[3] = 50;
  blink_color[0][RED] = 0;
  blink_color[0][GREEN] = 0;
  blink_color[0][BLUE] = 0;
  color[0][RED] = 255;
  color[0][GREEN] = 255;
  color[0][BLUE] = 255;
  turnOnPeripheral(0);
  turnOnPeripheral(1);
  turnOnPeripheral(2);
}

void setup()
{
  readingOperation = false;
  numBytesRead = 0;
  init_interrupts();
  for(int ledIndex = 0; ledIndex < NUM_MONO_LEDS; ledIndex++)
  {
   blink_state[ledIndex] = NOT_BLINKING;
   pinMode(monoPins[ledIndex], OUTPUT);
   turnOffPeripheral(ledIndex);
  }

  byte allColorPins[] = {COLOR_LED_PINS};
  for(int ledIndex = 0; ledIndex < NUM_COLOR_LEDS; ledIndex++)
  {
    blink_state[ledIndex + NUM_MONO_LEDS] = NOT_BLINKING;
    for(int colorIndex = 0; colorIndex < 3; colorIndex++)
    {
     pinMode(colorPins[ledIndex][colorIndex], OUTPUT);
     colorPins[ledIndex][colorIndex] = allColorPins[(ledIndex * 3) + colorIndex];
    }
    turnOffPeripheral(ledIndex + NUM_MONO_LEDS);
  }
  
  Serial.begin(9600);
  btSerial.begin(9600);
  
  Serial.println("Serial Communication Initialized");
  Serial.println();
}

void loop()
{
  interrupts();

  if(fiveMinuteTimerOn)
  {
    long ms = (millis() - fiveMinuteTimerMs);
    int seconds = ms / 1000;
    if(seconds > GREEN_OFF)
    {
      turnOffPeripheral(0);
    }
    if(seconds > YELLOW_OFF)
    {
      turnOffPeripheral(1);
    }
    if(seconds > RED_OFF)
    {
      turnOffPeripheral(2);
    }
    if((seconds > BLINK_START) && (blink_state[3] == NOT_BLINKING))
    {
      blink_state[3] = BLINKING_AND_ON;
      lastBlinkTransitionTime[3] = 0;
    }
    if(seconds > SECONDS_ON)
    {
      turnOffPeripheral(3);
      blink_state[3] = NOT_BLINKING; 
      fiveMinuteTimerOn = true;
    }
  }

  for(int i = 0; i < (NUM_MONO_LEDS + NUM_COLOR_LEDS); i++)
  {
    if((blink_state[i] == BLINKING_AND_ON) && ((millis() - lastBlinkTransitionTime[i]) > blink_on[i]))
    {
      lastBlinkTransitionTime[i] = millis();
      blink_state[i] = BLINKING_AND_OFF;
      turnOnPeripheralBlinkState(i);
    }
    else if((blink_state[i] == BLINKING_AND_OFF) && ((millis() - lastBlinkTransitionTime[i]) > blink_off[i]))
    {
      lastBlinkTransitionTime[i] = millis();
      blink_state[i] = BLINKING_AND_ON;
      turnOnPeripheral(i);
    }
  }

  char lastByte;
 
  if (btSerial.available() > 0)
  {
    lastByte = btSerial.read();
    if(!readingOperation)
    {
      readingOperation = true;
      Serial.println("Started reading operation.");
      operationIn = (byte*) malloc(1 + bytesInOpcode(lastByte));
      operationIn[0] = lastByte;
      numBytesRead = 0;
    }
    else
    {
      Serial.println("Read operand byte");
      numBytesRead++;
      operationIn[numBytesRead] = lastByte;
      if(numBytesRead == bytesInOpcode(operationIn[0]))
      {
        Serial.println("Finished reading operation.");
        Serial.println(opToString(operationIn));
        readingOperation = false;
        doOperation(operationIn); 
      }
    }    
  }
}

byte* doOperation(byte operation[])
{
  byte opcode = operation[0];
  
  switch(opcode)
  {
    case(OFF) : blink_state[operation[1]] = NOT_BLINKING; turnOffPeripheral(operation[1]); fiveMinuteTimerOn = false; break;
    case(ON) : blink_state[operation[1]] = NOT_BLINKING; turnOnPeripheral(operation[1]); break;
    case(BRIGHT) : brightness[operation[1]] = operation[2]; break;
    case(COLOR) : color[operation[1] - NUM_MONO_LEDS][RED] = operation[2]; color[operation[1] - NUM_MONO_LEDS][GREEN] = operation[3]; color[operation[1] - NUM_MONO_LEDS][BLUE] = operation[4]; break;
    case(BLINK) : blink_state[operation[1]] = BLINKING_AND_ON; blink_on[operation[1]] = 256 * ((unsigned short)operation[2]) + (unsigned short)operation[3]; blink_off[operation[1]] = (((short)operation[4]) * 256) + operation[5]; lastBlinkTransitionTime[operation[1]] = millis(); Serial.println(blink_on[operation[1]]);break;
    case(BLINK_BRIGHT) : blink_brightness[operation[1]] = operation[2]; break;
    case(BLINK_COLOR) : blink_color[operation[1] - NUM_MONO_LEDS][RED] = operation[2]; blink_color[operation[1] - NUM_MONO_LEDS][GREEN] = operation[3]; blink_color[operation[1] - NUM_MONO_LEDS][BLUE] = operation[4]; break;
    case(PUSH_HIGH) : digitalWrite(operation[1], 255); break;
    case(PUSH_LOW) : digitalWrite(operation[1], 0); break;
    case(PUSH_ANALOG) : analogWrite(operation[1], operation[2]); break;
    case(GET_DIGITAL) : break;
    case(GET_ANALOG) : break;
    case(FIVE_MINUTE_RED_TO_GREEN) : initFiveMinuteTimer(); break;
  }
}

void turnOnPeripheral(byte peripheralId)
{
  if(peripheralId < NUM_MONO_LEDS)
  {
    digitalWrite(monoPins[peripheralId], HIGH); 
  }
  else
  {
    peripheralId = peripheralId - NUM_MONO_LEDS;
    analogWrite(colorPins[peripheralId][RED], color[peripheralId][RED]);
    analogWrite(colorPins[peripheralId][GREEN], color[peripheralId][GREEN]);
    analogWrite(colorPins[peripheralId][BLUE], color[peripheralId][BLUE]);
  }
  
}

void turnOnPeripheralBlinkState(byte peripheralId)
{
  if(peripheralId < NUM_MONO_LEDS)
  {
    digitalWrite(monoPins[peripheralId], LOW); 
  }
  else
  {
    peripheralId = peripheralId - NUM_MONO_LEDS;
    analogWrite(colorPins[peripheralId][RED], blink_color[peripheralId][RED]);
    analogWrite(colorPins[peripheralId][GREEN], blink_color[peripheralId][GREEN]);
    analogWrite(colorPins[peripheralId][BLUE], blink_color[peripheralId][BLUE]);
  }
  
}

void turnOffPeripheral(byte peripheralId)
{
  if(peripheralId < NUM_MONO_LEDS)
  {
    digitalWrite(monoPins[peripheralId], LOW); 
  }
  else
  {
    peripheralId = peripheralId - NUM_MONO_LEDS;
    analogWrite(colorPins[peripheralId][RED], 0);
    analogWrite(colorPins[peripheralId][GREEN], 0);
    analogWrite(colorPins[peripheralId][BLUE], 0);
  }
  
}
