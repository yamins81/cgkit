# RenderMan demo

# Material definition that will be used for all objects
mat = GLMaterial(
    diffuse = (1,1,1),
    texture = GLTexture("uvmap.png",
                        mode = GL_MODULATE)
)

# Create a camera...
TargetCamera(
    pos    = (1,7,3),
    target = (0,0,1)
)

# ...and a spot light
lgt = GLTargetSpotLight(
    pos    = (2,4,8),
    target = (0,0,0),
    intensity = 0.9
)
# The following line is used by the RenderMan exporter to create a shadow
# map. The value is a tuple (map resolution, width, blur, bias, samples)
lgt.shadowmap = (512, 1.0, 0.01, 0.02, 32)

# Create a distant light (that does not cast shadows)
GLTargetDistantLight(
    pos    = (-5,0,10),
    target = (0,0,0),
    intensity = 0.25
)

# Create a couple of objects...

Plane(
    lx  = 10,
    ly  = 10,
    pos = (0,0,0),
    material = mat
)

Sphere(
    radius = 1.0,
    pos    = (-2,0,1),
    material = mat
)

CCylinder(
    radius = 0.5,
    length = 2.0,
    pos    = (0,0,1.5),
    material = mat
)

Box(
    lx  = 2.0,
    ly  = 0.75,
    lz  = 1.5,
    pos = (2,0,1),
    material = mat
)
