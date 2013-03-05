/*Handles external button presses from forward and backward button press*/

boolean back = false;
boolean forward = false;
volatile byte opcode = 0;


void init_interrupts(){
  attachInterrupt(0,forwardPressed, RISING);
}

//Called when a forward press is recieved and increments the opcode value
void forwardPressed(){
 if(opcode<12){
   opcode = opcode + 1;
 }else{
   opcode = 0;
 }
 Serial.print(opcode);
}
byte getOpcode(){
  return opcode;
}
