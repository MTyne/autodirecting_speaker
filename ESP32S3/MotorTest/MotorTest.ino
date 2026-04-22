#define PIN_A 1
#define PIN_B 2
#define PIN_ENABLE 3

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  analogWrite(PIN_ENABLE, 255);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_A, HIGH);
  analogWrite(PIN_ENABLE, 255);
  Serial.print("0.0\n");
  delay(1000);

  analogWrite(PIN_ENABLE, 0);
  Serial.print("0..\n");
  delay(1000);

  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, HIGH);
  analogWrite(PIN_ENABLE, 255);
  Serial.print(".0o\n");
  delay(1000);
  
  analogWrite(PIN_ENABLE, 0);
  Serial.print(".0.\n");
  delay(1000);
}
