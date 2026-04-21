#include <Arduino.h>
#include "motor_control.h"

#define PIN_CW 1
#define PIN_CCW 2
#define PIN_ENABLE 3
#define WAIT_MULT 1500
#define PWM_CYCLE 1.0f
#define POWER 255
#define POWERNT 0

void motor_init() {
  pinMode(PIN_CW, OUTPUT);
  pinMode(PIN_CCW, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_CW, LOW);
  digitalWrite(PIN_CCW, LOW);
  digitalWrite(PIN_ENABLE, LOW);
}

void motor_control(float val) {
  if (val > 0.0f) {
    digitalWrite(PIN_CW, POWER);
    digitalWrite(PIN_ENABLE, POWER);
  }
  if (val < 0.0f) {
    digitalWrite(PIN_CCW, POWER);
    digitalWrite(PIN_ENABLE, POWER);
    val = -val;
  }
  delay((uint32_t)(WAIT_MULT * val));
  digitalWrite(PIN_CW, LOW);
  digitalWrite(PIN_CCW, LOW);
  digitalWrite(PIN_ENABLE, LOW);
}