#include <camera.h>
#include <motor_control.h>
float cw_deadzone = 0.15;
float ccw_deadzone = -0.15;
#define NUM_AVERAGES 2
#define MOTOR_MULT 300
#define FRAME_RATE 5
#define THRESHOLD 0.5f


float chunk_average(float value, bool reset) {
  static float values[NUM_AVERAGES];
  static uint8_t index = 0;
  if (reset) {
    for (uint8_t i=0; i<NUM_AVERAGES;i++) {
      values[i] = 0;
      index = 0;
    }
  }
  float value_sum = 0;
  values[index] = value;
  index = (index + 1) % NUM_AVERAGES;
  for (uint8_t i=0; i<NUM_AVERAGES;i++) {
    value_sum += values[i];
  }
  if (index == 0) {
    return value_sum / NUM_AVERAGES;
  }
  return 0.0f;
}

float decider(float value) {
  if (value > cw_deadzone) {
    return value-cw_deadzone;
  }
  if (value < ccw_deadzone) {
    return value-ccw_deadzone;
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
  uint8_t thresh = 255 - (255 - camera_get_average()) * THRESHOLD;
  camera_special_print(8, thresh);
  camera_get_point(camera_get_average(), &row, &col);
  average = chunk_average(col, false);
  camera_return();

  Serial.printf("row: %f col: %f average: %f thresh: %u\n", row, col, average, thresh);

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
    motor_control((int16_t)(decider(average) * MOTOR_MULT));
    chunk_average(0.0f, true);
  }
  delay((int)(1.0f/FRAME_RATE*1000));
}
