void camera_init();
void camera_capture();
void camera_return();
void camera_get_point(const uint8_t minimum, float * row, float * col);
uint8_t camera_get_average();
void camera_print(const uint16_t ratio);
void camera_special_print(const uint16_t ratio, const uint8_t thresh);