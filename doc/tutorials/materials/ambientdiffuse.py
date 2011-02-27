# Ambient color vs diffuse color

TargetCamera(
    pos = (0,-10,0),
    fov = 30
)

GLTargetDistantLight(
    pos = (0,-1,1),
    ambient = (1,1,1),
    diffuse = (1,1,1),
)

for i in range(5):
    v = i/4.0
    for j in range(5):
        u = j/4.0
        Sphere(pos = (j-2,0,i-2),
               radius = 0.3,
               segmentsu = 128,
               segmentsv = 64,
               material = GLMaterial(ambient = u*vec3(0.5,0.5,1),
                                     diffuse = v*vec3(1,0.5,0.5)))
