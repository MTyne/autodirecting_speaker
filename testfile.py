import gpiozero
import picamera2
import time
import numpy

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



led1 = gpiozero.PWMLED(17)
led2 = gpiozero.PWMLED(27)

"""
size = 10
step = 0.01
try:
    while True:
        for i in range(size):
            led1.value = i / size
            time.sleep(step)
        for i in range(size):
            led2.value = i / size
            time.sleep(step)
        for i in range(size):
            led1.value = 1 - i / size
            time.sleep(step)
        for i in range(size):
            led2.value = 1 - i / size
            time.sleep(step)
except KeyboardInterrupt:
    print()"""

picam2 = picamera2.Picamera2()
picam2.start()
try:
    while True:
        image = numpy.asarray(picam2.capture_image("main").resize((32,20)))
        mean1 = float(numpy.max(image[:,0:8]))
        mean2 = float(numpy.max(image[:,24:32]))
        led1.value = mean1 / 256
        led2.value = mean2 / 256
        print_array(image)
except KeyboardInterrupt:
    print()