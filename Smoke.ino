const int smokeSensor = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int digitalNumber = analogRead(smokeSensor);
  Serial.println(digitalNumber);
  delay(500);
}
