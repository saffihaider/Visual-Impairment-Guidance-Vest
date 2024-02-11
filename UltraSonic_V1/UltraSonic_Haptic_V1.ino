// Ultrasonic pins
const int trigPin = 15; // Pin used to trigger ranging
const int echoPin1 = 2; // Ultrasonic 1 output 
const int echoPin2 = 4; // Ultrasonic 2 output 

const int threshold = 50; // Minimum distance for obstacle to be registered
const int delaytime = 500; // Delay between measurements
const int iterations = 1; // Averaging factor (number of measurements to be averaged per cycle)
int distance1, distance2;

//Haptic Motor Controls
#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv1;

//Haptic Pins
#define PWM_PIN_1 18 // Pin motor 2
#define PWM_PIN_2 12 // Pin motor 3
#define PWM_PIN_3 14 // Pin motor 4

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(echoPin2, INPUT);  
  Serial.begin(9600); // Starts the serial communication
  
  if (!drv1.begin()) {
      Serial.println("Failed to find motors ");
      while (1);
    }

  pinMode(PWM_PIN_1, OUTPUT); 
  pinMode(PWM_PIN_2, OUTPUT);
  pinMode(PWM_PIN_3, OUTPUT);

  Serial.println("Haptic initialized");
}

void loop() {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  
  // Output UltraSonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(970);
  
  // Scan for returned wave and measure pulse widths to calculate distance
  distance1 = pulseIn(echoPin1, HIGH) * 0.034 / 2.0;
  distance2 = pulseIn(echoPin2, HIGH) * 0.034 / 2.0;
  Serial.println(distance1);
  Serial.println(distance2);

  // Compare distance to threshold
  bool middle = (distance1 < threshold) && (distance2 < threshold);
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);

  // Trigger respective haptic motor
  if (middle) {
    drv1.setWaveform(8, 14);
    drv1.go();
    analogWrite(PWM_PIN_1, 255); // Set PWM duty cycle
    analogWrite(PWM_PIN_2, 255); // 255 for max vibration
    analogWrite(PWM_PIN_3, 255);
    delay(10);
  }
  else if (left) {
    Serial.println("left");
    drv1.setWaveform(8, 14);
    drv1.go();
    analogWrite(PWM_PIN_1, 255); // Set PWM duty cycle
    delay(10);

  }
  else if (right) {
    Serial.println("right");
    
    analogWrite(PWM_PIN_2, 255); // 255 for max vibration
    analogWrite(PWM_PIN_3, 255); 
    delay(10); 
  }
  else {
    Serial.println("N/A");
    // Add haptic controls here
  }
}
