# Alpha blending example

from OpenGL.GL import *
import Image, ImageDraw

TargetCamera(
    pos = (0,-3,0),
    fov = 30
)

Plane(
    # Rotate the plane so that it lies in the XZ plane
    rot = mat3().fromEulerXYZ(radians(90), 0, 0),
    
    material = GLMaterial(
                  diffuse = (1,1,1,1),
                  blend_factors = (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
                  texture = GLTexture(
                                imagename = "alphatex.png",
                                internalformat = GL_RGBA,
                                mode = GL_REPLACE,
                                # mirror the image
                                transform = mat4().scaling(vec3(1, -1, 1))
                            )
               )
)

# Set some spheres as background
mat = GLMaterial(diffuse=(0.8,0.8,1.0), emission=(0.3,0.3,0.3))
for i in range(5):
    for j in range(5):
        x = 0.3*(i-2)
        y = 0.3*(j-2)
        Sphere(pos=(x, 2, y), radius=0.2, material=mat, segmentsu=32, segmentsv=16)

