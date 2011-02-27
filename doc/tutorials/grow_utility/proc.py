# Generating spheres
#
# File: proc.py

def proc(P, **keyargs):
    r = 0.02
    RiTransformBegin()
    RiTranslate(P)
    RiSphere(r,-r,r,360)
    RiTransformEnd()

