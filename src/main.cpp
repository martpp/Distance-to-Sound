#include <Arduino.h>
#include <Ultrasonic.h>

// Pin mappings for the ATmega32 with MightyCore
// PA0 = 24
// PA1 = 25
// PA2 = 26
// PA3 = 27
// PA4 = 28
// PA5 = 29
// PA6 = 30
// PA7 = 31
// PB0 = 0
// PB1 = 1 // LED_BUILTIN
// PB2 = 2
// PB3 = 3
// PB4 = 4
// PB5 = 5
// PB6 = 6
// PB7 = 7
// PC0 = 16
// PC1 = 17
// PC2 = 18
// PC3 = 19
// PC4 = 20
// PC5 = 21
// PC6 = 22
// PC7 = 23
// PD0 = 8 // D0
// PD1 = 9 // D1
// PD2 = 10 // D2
// PD3 = 11 // D3
// PD4 = 12 // D4
// PD5 = 13 // D5
// PD6 = 14 // D6
// PD7 = 15 // D7
// PE0 = 32
// PE1 = 33
// PE2 = 34
// PE3 = 35
// PE4 = 36
// PE5 = 37
// PE6 = 38

#define LED_BUILTIN 1

// pin definitions for 3 HC-RS04
#define trigPin0 11 // D3
#define echoPin0 10 // D2
#define trigPin1 13 // D5
#define echoPin1 12 // D4
#define trigPin2 15 // D7
#define echoPin2 14 // D6

// define pin for lm386-amp
#define ampPin 24

Ultrasonic ultrasonic0(trigPin0, echoPin0);
Ultrasonic ultrasonic1(trigPin1, echoPin1);
Ultrasonic ultrasonic2(trigPin2, echoPin2);

unsigned int duration_ms;
unsigned int last_time_ms;

class MovingAverageFilter {
public:
  MovingAverageFilter(int window_size)
      : windowsize(window_size), index(0), sum(0.0f) {
    inputhistory = new float[windowsize];
    for (int i = 0; i < windowsize; i++) {
      inputhistory[i] = 0.0f;
    }
  }

  ~MovingAverageFilter() { delete[] inputhistory; }

  float update(float input) {
    sum -= inputhistory[index];       // remove the oldest value from the sum
    inputhistory[index] = input;      // insert the new value
    sum += input;                     // add the new value to the sum
    index = (index + 1) % windowsize; // move to the next position
    return sum / windowsize;
  }

private:
  float *inputhistory;
  int windowsize;
  int index;
  float sum;
};

int windowsize = 4;
MovingAverageFilter filter_0(windowsize);
MovingAverageFilter filter_1(windowsize);
MovingAverageFilter filter_2(windowsize);

float fdist0, fdist1, fdist2;

void setup() {
  Serial.begin(9600);
  pinMode(ampPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned int current_time_ms = millis();
  fdist0 = filter_0.update(ultrasonic0.read());
  fdist1 = filter_1.update(ultrasonic1.read());
  fdist2 = filter_2.update(ultrasonic2.read());
  unsigned int distance = min(fdist0, min(fdist1, fdist2));

  Serial.print(">dist0:");
  Serial.println(ultrasonic0.read());
  Serial.print(">dist0 filtered:");
  Serial.println(fdist0);
  Serial.print(">dist1:");
  Serial.println(ultrasonic1.read());
  Serial.print(">dist1 filtered:");
  Serial.println(fdist1);
  Serial.print(">dist2:");
  Serial.println(ultrasonic2.read());
  Serial.print(">dist2 filtered:");
  Serial.println(fdist2);

  // map distance to a duration value between 200 and 2000ms
  unsigned int duration1 = map(distance, 10, 100, 50, 1000);

  if (current_time_ms - last_time_ms > duration1 && distance < 75 &&
      distance > 10) {
    last_time_ms = current_time_ms;
    // play a tone with the duration
    // Change the amplitude of the tone to 1/2 of the max volume

    tone(ampPin, 500, duration1 / 2);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(100);
}