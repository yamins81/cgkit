######################################################################
# Demonstrating the grow utility
#
# File: model1.py
######################################################################

# The generated points will lie on the surface of this torus
Torus(
    major = 1.0,
    minor = 0.3,
    segmentsu = 32,
    segmentsv = 16,
    visible = False
)

# The following objects are only necessary for rendering:

TargetCamera(
    pos = (0,-3,2),
    target = (0,0,-0.2),
    fov = 40
)

SpotLight3DS(
    pos = (-2,-3,2),
    target = (0,0,0),
    falloff = 40,
    hotspot = 35,
    shadowed = True,
    shadow_size = 1024,
    shadow_filter = 2.0,
    shadow_bias = 0.001
)

RIBArchive(
    filename = "spheres.rib",
    material = RMMaterial( surface = RMShader("plastic"), color=(0.5,1,0.5) )
)
