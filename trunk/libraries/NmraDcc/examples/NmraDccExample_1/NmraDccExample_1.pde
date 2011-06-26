#include <NmraDcc.h>

// This Example shows how to use the library as a DCC Accessory Decoder or a DCC Signalling Decoder
// It responds to both the normal DCC Turnout Control packets and the newer DCC Signal Aspect packets 
// You can also print every DCC packet by uncommenting the notifyDccMsg() function below

NmraDcc  Dcc ;
DCC_MSG  Packet ;

const int DccAckPin = 3 ;

// This function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read 
void notifyCVAck(void)
{
  Serial.println("notifyCVAck") ;
  
  digitalWrite( DccAckPin, HIGH );
  delay( 6 );  
  digitalWrite( DccAckPin, LOW );
}

// Uncomment to print all DCC Packets
/*void notifyDccMsg( DCC_MSG * Msg)
{
  Serial.print("notifyDccMsg: ") ;
  for(uint8_t i = 0; i < Msg->Size; i++)
  {
    Serial.print(Msg->Data[i], HEX);
    Serial.write(' ');
  }
  Serial.println();
}*/

// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccState( uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State)
{
  Serial.print("notifyDccAccState: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(',');
  Serial.print(BoardAddr,DEC) ;
  Serial.print(',');
  Serial.print(OutputAddr,DEC) ;
  Serial.print(',');
  Serial.println(State, HEX) ;
}

// This function is called whenever a DCC Signal Aspect Packet is received
void notifyDccSigState( uint16_t Addr, uint8_t OutputIndex, uint8_t State)
{
  Serial.print("notifyDccSigState: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(',');
  Serial.print(OutputIndex,DEC) ;
  Serial.print(',');
  Serial.println(State, HEX) ;
}

void setup()
{
  Serial.begin(115200);
  
  // Configure the DCC CV Programing ACK pin for an output
  pinMode( DccAckPin, OUTPUT );

  Serial.println("NMRA DCC Example 1");
  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, FLAGS_OUTPUT_ADDRESS_MODE | FLAGS_DCC_ACCESSORY_DECODER | FLAGS_ENABLE_INT0_PULL_UP, 0 );
  Serial.println("Init Done");
}

void loop()
{
  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();
}