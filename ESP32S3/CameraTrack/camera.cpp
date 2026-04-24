#include <Arduino.h>
#include "esp_camera.h"
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
#include "camera_pins.h"
#include "camera.h"

// hidden logic
camera_fb_t *buf = NULL;

void _print_buffer(camera_fb_t * buf, const uint8_t ratio, const char* intensity_char, const uint16_t intensity_len) {
  char intensitybuffer[1024];
  size_t width = buf->width;
  size_t height = buf->height;
  for (uint16_t row=0;row<width/ratio;row++) {
    for (uint16_t col=0;col<height/ratio;col++) {
      uint16_t val = (buf->buf[col*ratio+row*ratio*height] * (intensity_len-1)) / 256;
      intensitybuffer[col*2] = intensity_char[val];
      intensitybuffer[col*2+1] = intensity_char[val];
    }
    intensitybuffer[height/ratio*2] = '\0';
    Serial.print(intensitybuffer);
    Serial.println();
  }
}

uint8_t _get_brightest(camera_fb_t * buf, uint16_t * row, uint16_t * col) {
  uint8_t maximum = 0;
  uint32_t sum_row = 0;
  uint32_t sum_col = 0;
  uint16_t sum_count = 0;
  size_t width = buf->width;
  size_t height = buf->height;
  for (uint16_t roww=0;roww<height;roww++) {
    for (uint16_t coll=0;coll<width;coll++) {
      uint16_t val = buf->buf[coll+roww*height];
      if (maximum < val) {
        sum_row = roww;
        sum_col = coll;
        sum_count = 1;
        maximum = val;
      }
      if (maximum == val) {
        sum_row += roww;
        sum_col += coll;
        sum_count += 1;
      }
    }
  }
  if (sum_count) {
    if (row != NULL) {
      *row = sum_row / sum_count;
    }
    if (col != NULL) {
      *col = sum_col / sum_count;
    }
    return maximum;
  }
  return 0;
}

uint8_t _get_average(camera_fb_t * buf) {
  uint64_t total_sum = 0;
  uint16_t sum_count = 0;
  size_t width = buf->width;
  size_t height = buf->height;
  uint16_t row, col;
  for (row=0;row<height;row++) {
    for (col=0;col<width;col++) {
      uint16_t val = buf->buf[col+row*height];
      total_sum += val;
      sum_count += 1;
    }
  }
  return total_sum / sum_count;
}

// Visible functions
void camera_init() {  
  if (!Serial) {
    Serial.begin(115200);
    delay(1000);
  }
  Serial.println("Starting...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  //config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.pixel_format = PIXFORMAT_GRAYSCALE; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.frame_size = FRAMESIZE_96X96;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  sensor_t *s = esp_camera_sensor_get();
  s->set_brightness(s, -2);
  s->set_saturation(s, 2);

  /*buf = esp_camera_fb_get();
  Serial.printf("%zu %zu %zu", buf->width, buf->height, buf->len);
  Serial.println(" ");
  esp_camera_fb_return(buf);
  buf = NULL;*/
  Serial.print("Setup Finished...\n");
}

void camera_capture() {
  buf = esp_camera_fb_get();
}

void camera_return() {
  esp_camera_fb_return(buf);
}

void camera_get_point(const uint8_t minimum, float * row, float * col) {
  uint16_t nrow, ncol;
  *col = 0.0f; *row = 0.0f;
  if (!buf) {
    return;
  }
  uint8_t maximum = _get_brightest(buf, &nrow, &ncol);
  if (minimum <= maximum) {
    *col = (ncol * 2) / (float)buf->width - 1.0f;
    *row = (nrow * 2) / (float)buf->height - 1.0f;
  }
}

uint8_t camera_get_average() {
  return _get_average(buf);
}

void camera_print(const uint16_t ratio) {
  char intensity[] = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
  _print_buffer(buf, ratio, intensity, sizeof(intensity));
}

void camera_special_print(const uint16_t ratio, const uint8_t thresh) {
  char intensity[257];
  uint8_t maximum = _get_brightest(buf, NULL, NULL);
  for (uint16_t i=0;i<256;i++) {
    intensity[i] = ' ';
    if (i>=thresh) {
      intensity[i] = 'o';
    }
    if (i>=maximum-1) {
      intensity[i] = '@';
    }
  }
  intensity[256] = '\0';
  _print_buffer(buf, ratio, intensity, strlen(intensity));
}
