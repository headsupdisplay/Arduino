#include "MinHudOpcodes.h"
#include <string.h>

int bytesInOpcode(byte opcode)
{
	return {1, 1, 4, 2, 5, 4, 2, 1, 1, 1, 2, 1, 1}[opcode];
}

char* opcodeToString(byte opcode)
{
	switch(opcode)
	{
		case 0: return "OFF"; break;
		case 1: return "ON"; break;
		case 2: return "COLOR"; break;
		case 3: return "BRIGHT"; break;
		case 4: return "BLINK"; break;
		case 5: return "BLINK_COLOR"; break;
		case 6: return "BLINK_BRIGHT"; break;
		case 7: return "SENSOR"; break;
		case 8: return "PUSH_HIGH"; break;
		case 9: return "PUSH_LOW"; break;
		case 10: return "PUSH_ANALOG"; break;
		case 11: return "GET_DIGITAL"; break;
		case 12: return "GET_ANALOG"; break;
	}
	return "UNKNOWN";
}

char* opToString(byte[] operation)
{
	int bytes = bytesInOpcode(operation[0]);

	char* opcodeName = opcodeToString(operation[0]);
	char opString[bytes + sizeOf(opCodeName) + 1];
	memcpy(opString, opCodeName, sizeOf(opCodeName));

	for(int i = sizeOf(opCodeName); i < bytes + sizeOf(opCodeName); i++)
	{
		opString[i] = operation[i + 1];
	}

	return opString;
}