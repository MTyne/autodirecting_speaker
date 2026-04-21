#include <camera.h>
#include <motor_control.h>
float cw_deadzone = 0.5;
float ccw_deadzone = -0.5;
#define NUM_AVERAGES 25


float rolling_average(float value, bool reset) {
  static float values[NUM_AVERAGES];
  static uint8_t index = 0;
  if (reset) {
    for (uint8_t i=0; i<NUM_AVERAGES;i++) {
      values[i] = 0;
    }
  }
  float value_sum = 0;
  values[index] = value;
  index = (index + 1) % NUM_AVERAGES;
  for (uint8_t i=0; i<NUM_AVERAGES;i++) {
    value_sum += values[i];
  }
  return value_sum / NUM_AVERAGES;
}

float decider(float value) {
  if (value > cw_deadzone) {
    return 1.0f;
  }
  if (value < ccw_deadzone) {
    return -1.0f;
  }
  return 0.0f;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.setDebugOutput(true);
  delay(1000);
  camera_init();
  motor_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  float row, col, average;
  camera_capture();
  camera_print(20);
  camera_get_point(1, &row, &col);
  average = rolling_average(col, false);
  camera_return();

  Serial.printf("%f %f\n", row, col);
  
  Serial.printf("%f ", average);
  if (decider(average) > 0.0f) {
    Serial.printf("CLOCKWISE with %f\n", decider(average));
  }
  if (decider(average) < 0.0f) {
    Serial.printf("COUNTER CLOCKWISE with %f\n", decider(average));
  }
  if (decider(average) == 0.0f) {
    Serial.printf("STILL with %f\n", decider(average));
  }
  if (decider(average) != 0.0f) {
    motor_control(decider(average));
    rolling_average(0.0f, true);
  }
  Serial.println();
  delay(100);
}
