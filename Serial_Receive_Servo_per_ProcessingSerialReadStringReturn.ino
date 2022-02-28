#define DIR_PIN          2
#define STEP_PIN         3
#define ENABLE_PIN       4

#define STEP_HIGH        PORTD |=  0b00001000;
#define STEP_LOW         PORTD &= ~0b00001000;

#define TIMER1_INTERRUPTS_ON    TIMSK1 |=  (1 << OCIE1A);
#define TIMER1_INTERRUPTS_OFF   TIMSK1 &= ~(1 << OCIE1A);

#include "ServoTimer2.h"

#define pitchpin 12
#define BUFFER_SIZE 20

char incommingBuffer[BUFFER_SIZE];
char incomming; // primary buffer to store single incommning bytes
int incommingCounter = 0; //counter for counting the positions inside the buffer
int x;
int y;

ServoTimer2 MyServo; // ServoTimer2 Library to avoid communication Problems with Stepper Motor and Serial

unsigned int c0;


void setup() {
  Serial.begin(115200);
  Serial.setTimeout (10);
  pinMode(10, OUTPUT);
  int val1;
  MyServo.attach(11);
  pinMode(STEP_PIN,   OUTPUT);
  pinMode(DIR_PIN,    OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 1000;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= ((1 << CS11) | (1 << CS10));
  interrupts();
  c0 = 1600; // was 2000 * sqrt( 2 * angle / accel )
}


void readSerial()
{
  while (Serial.available())
  {
    incomming = Serial.read(); // read single incommning bytes

    if (incomming != '\r') //if no carriage return is received proceed in reading the serial port
    {
      incommingBuffer[incommingCounter++] = incomming; // go on the next position in the buffer
    }
    else //read until a carriage ('\r') is received
    {
      incommingBuffer[incommingCounter] = '&#092;&#048;'; // set the last byte to NULL to sign it for the string operators

      char *a = strtok(incommingBuffer, ",.;"); // split the string after delimiters into tokens
      char *b = strtok(NULL, ",.;"); // ...
      char *c = strtok(NULL, ",.;"); // ...
      //char *d = strtok(NULL, ",.;"); // add another line if needed

      x = atoi(a); // convert the strings into integers
      y = atoi(b); // ...

      incommingCounter = 0; // reset the counter
      memset(incommingBuffer, 0, BUFFER_SIZE); //overwrite the incommingBuffer
    }
  }
}


volatile int dir = 0;
volatile unsigned int maxSpeed = 5;
volatile unsigned long n = 0;
volatile float d;
volatile unsigned long stepCount = 0;
volatile unsigned long rampUpStepCount = 0;
volatile unsigned long totalSteps = 0;
volatile int stepPosition = 0;

volatile bool movementDone = false;

ISR(TIMER1_COMPA_vect)
{
  if ( stepCount < totalSteps ) {
    STEP_HIGH
    STEP_LOW
    stepCount++;
    stepPosition += dir;
  }
  else {
    movementDone = true;
    TIMER1_INTERRUPTS_OFF
  }

  if ( rampUpStepCount == 0 ) { // ramp up phase
    n++;
    d = d - (2 * d) / (4 * n + 1);
    if ( d <= maxSpeed ) { // reached max speed
      d = maxSpeed;
      rampUpStepCount = stepCount;
    }
    if ( stepCount >= totalSteps / 2 ) { // reached halfway point
      rampUpStepCount = stepCount;
    }
  }
  else if ( stepCount >= totalSteps - rampUpStepCount ) { // ramp down phase
    n--;
    d = (d * (4 * n + 1)) / (4 * n + 1 - 2);
  }

  OCR1A = d;
}

void moveNSteps(long steps) {

  digitalWrite(DIR_PIN, steps < 0 ? HIGH : LOW);
  dir = steps > 0 ? 1 : -1;
  totalSteps = abs(steps);
  d = c0;
  OCR1A = d;
  stepCount = 0;
  n = 0;
  rampUpStepCount = 0;
  movementDone = false;

  TIMER1_INTERRUPTS_ON
}

void moveToPosition(long p, bool wait = true) {
  moveNSteps(p - stepPosition);

  while ( wait && ! movementDone );
}

void loop() {

  if (Serial.available()) { //if data is available to read
    String val = Serial.readString();
    Serial.println (val);

    /*
      Serial.print  (x);
      Serial.print  ("\t");
      Serial.print  (y);
    */
    if (val == 'x') {
      moveToPosition (y * 10);
      MyServo.write(x); //use servo
      digitalWrite(10, HIGH);
      delay(2000);
      digitalWrite(10, LOW); delay(200);
      MyServo.write(0);
      moveToPosition (0);
    }
  }
  /*delay(1200);
    Serial.println ("Test");
  */
}

// maxSpeed = 1000;

// while (true);
