######################################################################
# A simple static scene.
# Defining a material separately and using it for more than one object
######################################################################

TargetCamera(
    pos    = vec3(3,2,2),
    target = vec3(0,0,1)
)

GLPointLight( pos = (2,3,4), intensity=0.8 )
GLPointLight( pos = (2,-3,1), intensity=0.8 )

# Define a material for later use
yellow = GLMaterial(
             name      = "Yellow",
             diffuse   = (1, 0.9, 0.3),
             specular  = (1, 1, 1),
             shininess = 80
         )

# Define the geometry...

Box(
    name = "Bottom",
    lx  = 1.0,
    ly  = 1.0,
    lz  = 0.2,
    material = yellow
)

Box(
    name = "Top",
    lx  = 1.0,
    ly  = 1.0,
    lz  = 0.2,
    pos = vec3(0,0,2),
    material = yellow
)

Sphere(
    name = "Middle",
    radius = 0.5,
    pos    = vec3(0,0,1),
    scale  = vec3(1,1,2.2),
    material = GLMaterial(
                   diffuse = vec3(1,0,0),
                   specular = vec3(1,1,1),
                   shininess = 30
               )
)


