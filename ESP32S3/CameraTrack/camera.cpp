#include <Arduino.h>
#include "esp_camera.h"
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
#include "camera_pins.h"
#include "camera.h"
camera_fb_t *buf = NULL;

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

void _print_buffer(camera_fb_t * buf, uint8_t ratio) {
  char intensity[] = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
  //char intensity[] = "                                                               `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}I1tlu[neoZ5Yxjya]ESwqkP6hdVpOGbUAKXHmRD#$BgMNWQ%&@";
  char intensitybuffer[1024];
  size_t width = buf->width;
  size_t height = buf->height;
  uint16_t row, col;
  for (row=0;row<width/ratio;row++) {
    for (col=0;col<height/ratio;col++) {
      uint16_t val = buf->buf[col*ratio+row*ratio*height];
      val = val * (sizeof(intensity)-1) / 256;
      intensitybuffer[col*2] = intensity[val];
      intensitybuffer[col*2+1] = intensity[val];
    }
    intensitybuffer[col*2] = '\0';
    Serial.print(intensitybuffer);
    Serial.println();
  }
}

void _get_brightest(camera_fb_t * buf, const uint16_t ratio, uint16_t * row, uint16_t * col) {
  uint8_t maximum = 0;
  uint32_t sum_row = 0;
  uint32_t sum_col = 0;
  uint16_t sum_count = 0;
  size_t width = buf->width;
  size_t height = buf->height;
  uint16_t roww, coll;
  for (roww=0;roww<height/ratio;roww++) {
    for (coll=0;coll<width/ratio;coll++) {
      uint16_t val = buf->buf[coll*ratio+roww*ratio*height];
      if (maximum < val) {
        sum_row = roww*ratio;
        sum_col = coll*ratio;
        sum_count = 1;
        maximum = val;
      }
      if (maximum == val) {
        sum_row += roww*ratio;
        sum_col += coll*ratio;
        sum_count += 1;
      }
    }
  }
  *row = sum_row / sum_count;
  *col = sum_col / sum_count;
}

void camera_capture() {
  buf = esp_camera_fb_get();
}
void camera_return() {
  esp_camera_fb_return(buf);
}

void camera_get_point(const uint16_t ratio, float * row, float * col) {
  uint16_t nrow, ncol;
  if (!buf) {
    *row = 0.0f; *col = 0.0f;
    return;
  }
  _get_brightest(buf, ratio, &nrow, &ncol);
  *col = (ncol * 2) / (float)buf->width - 1.0f;
  *row = (nrow * 2) / (float)buf->height - 1.0f;
}

void camera_print(const uint16_t ratio) {
  _print_buffer(buf, ratio);
}
