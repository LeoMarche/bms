// Pinout
int input32 = A0;
int input64 = A1;
int input96 = A2;
int input128 = A3;
int chargeRelay = PD4;
int loadRelay = PD5;

// The following depends on the voltage
// dividers added on each input
float multiplier32 = 1.0;
float multiplier64 = 2.0;
float multiplier96 = 3.0;
float multiplier128 = 4.0;

float cellsVoltageUpLimit = 3.55;
float cellsVoltageDownLimit = 2.8;

float cellCompareUp;
float cellCompareDown;

// Variables initialization
long REF_VOLTAGE_MILLI = 0;
float REF_VOLTAGE = 0.0;

float val32 = 0.0;  // variable to store the value read
float val64 = 0.0;
float val96 = 0.0;
float val128 = 0.0;

void setup() {
  pinMode(chargeRelay, OUTPUT);
  pinMode(loadRelay, OUTPUT);
  digitalWrite(chargeRelay, HIGH);
  digitalWrite(loadRelay, LOW);
  Serial.begin(9600);
}

void loop() {
  REF_VOLTAGE_MILLI = readVcc();
  REF_VOLTAGE = REF_VOLTAGE_MILLI/1000.0;
  Serial.print("Ref voltage (mV) : ");
  Serial.println((int)REF_VOLTAGE_MILLI);

  cellCompareUp = cellsVoltageUpLimit*1023.0/REF_VOLTAGE;
  cellCompareDown = cellsVoltageDownLimit*1023.0/REF_VOLTAGE;
  Serial.print("Comparison against :");
  Serial.print(cellCompareUp);
  Serial.print(" , ");
  Serial.println(cellCompareDown);
  
  val32 = analogRead(input32)*multiplier32;
  val64 = analogRead(input64)*multiplier64;
  val96 = analogRead(input96)*multiplier96;
  val128 = analogRead(input128)*multiplier128;

  if(val32 < cellCompareDown | (val64-val32) < cellCompareDown | (val96-val64) < cellCompareDown | (val128-val96) < cellCompareDown){
    digitalWrite(loadRelay, LOW);
  } else {
    digitalWrite(loadRelay, HIGH);
  }

  if(val32 > cellCompareUp | (val64-val32) > cellCompareUp | (val96-val64) > cellCompareUp | (val128-val96) > cellCompareUp){
    digitalWrite(chargeRelay, HIGH);
  } else {
    digitalWrite(chargeRelay, LOW);
  }
  
  Serial.println(val32);
  Serial.println(val64);
  Serial.println(val96);
  Serial.println(val128);
  Serial.println(analogRead(A4));
  
  delay(5000);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
