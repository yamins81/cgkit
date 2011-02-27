# Show specular material settings

TargetCamera(
    pos = (0, -30, 0),
    fov = 10
)

GLTargetDistantLight(
    pos = (-0.2, -1, 1)
)

GLTargetDistantLight(
    pos = (1, 1, -1),
    diffuse = (0.5,0.5,1),
    intensity = 2
)

col = vec3(0.7, 0.3, 0)
for i in range(5):
    for j in range(5):
        Sphere(pos = (j-2,0,i-2),
               radius = 0.3,
               segmentsu = 128,
               segmentsv = 64,
               material = GLMaterial(diffuse = col,
                                     specular = mix(col, vec3(1), i/4.0),
                                     shininess = j*20+5))

