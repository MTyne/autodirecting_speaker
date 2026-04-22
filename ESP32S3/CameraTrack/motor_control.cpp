#include <Arduino.h>
#include "motor_control.h"

#define PIN_CW 1
#define PIN_CCW 2
#define PIN_ENABLE 3

void motor_init() {
  pinMode(PIN_CW, OUTPUT);
  pinMode(PIN_CCW, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_CW, LOW);
  digitalWrite(PIN_CCW, LOW);
  digitalWrite(PIN_ENABLE, LOW);
}

void motor_control(int16_t val) {
  if (val > 0.0f) {
    digitalWrite(PIN_CW, HIGH);
    digitalWrite(PIN_ENABLE, HIGH);
  }
  if (val < 0.0f) {
    digitalWrite(PIN_CCW, HIGH);
    digitalWrite(PIN_ENABLE, HIGH);
    val = -val;
  }
  delay(val);
  digitalWrite(PIN_CW, LOW);
  digitalWrite(PIN_CCW, LOW);
  digitalWrite(PIN_ENABLE, LOW);
}