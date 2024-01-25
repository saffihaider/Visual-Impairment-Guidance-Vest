// Ultrasonic pins
const int trigPin1 = 15;
const int echoPin1 = 2;
const int trigPin2 = 16;
const int echoPin2 = 4;

const int threshold = 50; // Minimum distance for obstacle to be registered
const int delaytime = 1000; // Delay between each pulse 
const int iterations = 3; // Averaging factor for each distance measurement

//const int middleLED = 5;
//const int leftLED = 18;
//const int rightLED = 19;

int distance1, distance2;

void setup() {
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  
  //pinMode(middleLED, OUTPUT);
  //pinMode(leftLED, OUTPUT);
  //pinMode(rightLED, OUTPUT);
  
  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  // Measure distances

  distance1 = getDistance(trigPin1, echoPin1, iterations);
  //distance1 = getDistance(trigPin1, echoPin1);
  delayMicroseconds(delaytime);
  distance2 = getDistance(trigPin2, echoPin2, iterations);
  //distance2 = getDistance(trigPin2, echoPin2);
  delayMicroseconds(delaytime);

  // Compare distance to threshold
  bool middle = (distance1 < threshold) && (distance2 < threshold);
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);

  // Trigger respective haptic motor
  if (middle) {
    Serial.println("middle");
//    LEDControl(middleLED, true);
//    LEDControl(leftLED, false);
//    LEDControl(rightLED, false);  
  }
  
  else if (left) {
    Serial.println("left");
//    LEDControl(middleLED, false);
//    LEDControl(leftLED, true);  
//    LEDControl(rightLED, false);
  }
  
  else if (right) {
    Serial.println("right");
//    LEDControl(middleLED, false);
//    LEDControl(leftLED, false);  
//    LEDControl(rightLED, true);
  }
  
  else {
    Serial.println("N/A");
//    LEDControl(middleLED, false);
//    LEDControl(leftLED, false);  
//    LEDControl(rightLED, false);
  }
}

// Triggers ultrasonic sensor, measures pulse width, and returns distance in cm
float getDistance(int trigPin, int echoPin) {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  
  // Output UltraSonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Scan for returned wave and calculate distance
  int duration = pulseIn(echoPin, HIGH);
  return (duration * 0.034 / 2.0);
}

// Averages the distance measurement
float getDistance(int trigPin, int echoPin, int iterations) {
  float avgDistance = 0;
  for (int i = 0; i < iterations; i++) {
    avgDistance += getDistance(trigPin, echoPin);
  }
  return avgDistance/iterations;
}

//void LEDControl(int LEDPin, bool condition) {
//  if (condition) {
//    digitalWrite(LEDPin, HIGH);
//  }
//  else{
//    digitalWrite(LEDPin, LOW);
//  }
//}
