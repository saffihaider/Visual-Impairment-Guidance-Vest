// defines pins numbers
const int trigPin1 = 2;
const int echoPin1 = 4;
const int trigPin2 = 16;
const int echoPin2 = 17;
const int middleLED = 5;
const int leftLED = 18;
const int rightLED = 6;
const int threshold = 100;

// defines variables
int distance1, distance2;

void setup() {
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  
  pinMode(middleLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  //distance1 = getDistance(trigPin1, echoPin1, 5);
  distance1 = getDistanceAvg(trigPin1, echoPin1, 5);
  // Prints the distance on the Serial Monitor
  Serial.print("Distance 1: ");
  Serial.println(distance1);
  delayMicroseconds(10);
  
  //distance2 = getDistance(trigPin2, echoPin2);
  distance2 = getDistanceAvg(trigPin2, echoPin2, 5);
  Serial.print("Distance 2: ");
  Serial.println(distance2);
  delayMicroseconds(10);
  //delay(500);
  
  bool middle = (distance1 < threshold) && (distance2 < threshold);
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);
  
  if (middle) {
    LEDControl(middleLED, true);
    LEDControl(leftLED, false);
    LEDControl(rightLED, false);  
  }
  
  else if (left) {
    LEDControl(middleLED, false);
    LEDControl(leftLED, true);  
    LEDControl(rightLED, false);
  }
  
  else if (right) {
    LEDControl(middleLED, false);
    LEDControl(leftLED, false);  
    LEDControl(rightLED, true);
  }
  
  else {
    LEDControl(middleLED, false);
    LEDControl(leftLED, false);  
    LEDControl(rightLED, false);
  }
}

int getDistance(int trigPin, int echoPin) {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Output ultraSonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Scan for returned wave and calculate distance
  int duration = pulseIn(echoPin, HIGH);
  return (duration * 0.034 / 2);
}

float getDistanceAvg(int trigPin, int echoPin, int iterations) {
  float avgDistance = 0;
  for (int i = 0; i < iterations; i++) {
    avgDistance += getDistance(trigPin, echoPin);
  }
  return avgDistance/iterations;
}

void LEDControl(int LEDPin, bool condition) {
  if (condition) {
    digitalWrite(LEDPin, HIGH);
  }
  else{
    digitalWrite(LEDPin, LOW);
  }
}
