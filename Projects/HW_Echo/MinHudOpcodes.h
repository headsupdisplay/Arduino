#ifndef MINHUDOPCODES_H
#define MINHUDOPCODES_H
 
int add(int x, int y); // function prototype for add.h

byte OFF = 0;
byte ON = 1;
byte COLOR = 2;
byte BRIGHTNESS = 3;
byte BLINK = 4;
byte BLINK_COLOR = 5;
byte BLINK_BRIGHTNESS = 6;
byte GET_SENSOR = 7;
byte PUSH_HIGH = 8;
byte PUSH_LOW = 9;
byte PUSH_ANALOG = 10;
byte GET_DIGITAL = 11;
byte GET_ANALOG = 12;

int bytesInOpcode(byte opcode);
char* opToString(byte[] operation);
char* opcodeToString(byte opcode)


 
#endif