######################################################################
# Using a texture map.
######################################################################

from math import *

TargetCamera(
    pos    = vec3(0,-4,0),
    target = vec3(0,0,0),
    fov    = 30
)

GLPointLight( pos = (0,-5,0), intensity=1.0 )

# Define a material for later use
mat = GLMaterial(
    name      = "Texture_mapped",
    diffuse   = (1, 1, 1),
    texture   = GLTexture("maps/uvmap.png")
)

# Define the geometry...

# Back plane as a polyhedron with one polygon...
p = Polyhedron(
    name = "back",
    pos = (-0.5, 0, -0.5),
    verts = [vec3(0,0,0), vec3(1,0,0), vec3(1,0,1), vec3(0,0,1)],
    polys = [[(0,1,2,3)]],
    material = mat
)
# Create and initialize texture coordinates.
# A texture coordinate slot must always be called "st" and of
# type "float[2]". The storage class will usually be VARYING or
# FACEVARYING (in the case of triangle meshes it could also be
# USER together with a slot "uniform int stfaces[3]" that contains
# the texture coordinate faces).
pg = p.geom
pg.newVariable("st", VARYING, FLOAT, 2)
st = pg.slot("st")
st[0] = (0,1)
st[1] = (1,1)
st[2] = (1,0)
st[3] = (0,0)

# The following plane could be used to replace the above polyhedron
#Plane(
#    name = "back",
#    lx = 1.0,
#    ly = 1.0,
#    rot = mat3().fromEulerXYZ(radians(-90), 0, 0),
#    material = mat
#)

# Left plane
d = sqrt(0.5**2/2)
Plane(
    name = "left",
    lx = 1.0,
    ly = 1.0,
    pos = vec3(-0.5-d, -d, 0),
    rot = mat3().fromEulerXYZ(radians(-90), radians(-45), 0),
    material = mat
)

# Right plane
d = sqrt(0.5**2/2)
Plane(
    name = "right",
    lx = 1.0,
    ly = 1.0,
    pos = vec3( 0.5+d, -d, 0),
    rot = mat3().fromEulerXYZ(radians(-90), radians(45), 0),
    material = mat
)

# Middle sphere
Sphere(
    radius = 0.2,
    pos = (0, -1.0, -0.6),
    segmentsu = 32,
    segmentsv = 16,
    material = mat
)
