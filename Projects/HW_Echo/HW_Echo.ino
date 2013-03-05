#line 1 "Serial_HW_Comm.ino"
#include <SoftwareSerial.h>

#include "Arduino.h"
void setup();
void loop();

SoftwareSerial btSerial(8, 9); // RX, TX
String messageToBT;
String messageFromBT;

void setup()
{
  
  messageToBT = "";
  messageFromBT = "";
  
  Serial.begin(9600);
  btSerial.begin(9600);
  
  Serial.println("Serial Communication Initialized");
  Serial.println();
  
}

void loop()
{
  
  char lastChar;
  
  if (Serial.available() > 0)
  {
    lastChar = Serial.read();
    
    Serial.print("Received ");
    Serial.print(lastChar);
    Serial.print(" from terminal.\n\n");
    
    if( lastChar == '\n' )
    {
      
      Serial.println("Sending message to bluetooth:");
      Serial.println(messageToBT);
      Serial.println();
      
      for(int i = 0; i < messageToBT.length(); i++)
      {
        btSerial.write(messageToBT[i]);
        Serial.print("Wrote ");
        Serial.print(messageToBT[i], BIN);
        Serial.print(" to the bluetooth modem.\n\n");
      }
      
      btSerial.write("\n");
      Serial.print("Wrote newline to the bluetooth modem.\n\n");
      
      messageToBT = "";
      
    }
    else
    {
      messageToBT = messageToBT + lastChar;
    }
    
  }
  
  if (btSerial.available() > 0)
  {
    Serial.println("Got SOMETHING!\n");
    lastChar = btSerial.read();
    
    Serial.print("Received ");
    Serial.print(lastChar);
    Serial.print(" from bluetooth modem.\n\n");
        
    if( lastChar == '\n' )
    {
      
      Serial.println("Message received from bluetooth:");
      Serial.println(messageFromBT);
      Serial.println();
      
      for(int i = 0; i < messageFromBT.length(); i++)
      {
        Serial.write(messageFromBT[i]);
        Serial.print("Wrote ");
        Serial.print(messageFromBT[i], BIN);
        Serial.print(" to the PC side serial.\n\n");
      }
      
      Serial.write("\n");
      
      messageFromBT = "";
      
    }
    else
    {
      messageFromBT = messageFromBT + lastChar;
    }
    
  }
  
}
