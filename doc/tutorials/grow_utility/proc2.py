# Generating disks
#
# File: proc2.py

from cgkit.noise import *

def proc2(P, F, **keyargs):
    r = 0.05
    RiTransformBegin()
    RiConcatTransform(F)
    RiDisk(r*snoise(5*P), r, 360)
    RiTransformEnd()

