import picamera2
import numpy
from PIL import Image

def print_array(array: numpy.ndarray):
    valuestring = str(b"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,^`'. ")
    valuecount = len(valuestring)
    rows = numpy.size(array, 0)
    cols = numpy.size(array, 1)
    print_string = ""
    for i in range(rows):
        row_string = ""
        for j in range(cols):
            value = int((1 - numpy.mean(array[i,j]) / 256) * (valuecount-1))
            row_string += valuestring[value]*2
        print_string += row_string + "\n"
    print(print_string)

def print_screen(size=(64,40)):
    array = numpy.asarray(capture_screen().resize(size))
    print_array(array)

def capture_screen() -> Image:
    return _CAM.capture_image("main")

def main():
    import time
    while True:
        print_screen()
        time.sleep(0.25)

_CAM = picamera2.Picamera2()
_CAM.start()

if __name__ == "__main__":
    main()
