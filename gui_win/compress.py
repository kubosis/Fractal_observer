import os
from PIL import Image
import sys

filename = sys.argv[1]

im = Image.open("/home/kubos/Documents/Fractal_observer/gui_win/"+filename+".ppm")
im.save("/home/kubos/Documents/Fractal_observer/fractal_shots/"+filename+".png")
#os.remove("/home/kubos/Documents/Fractal_observer/gui_win/"+filename+".ppm")
