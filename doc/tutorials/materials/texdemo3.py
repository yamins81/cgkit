# Texture mapping example

TargetCamera(
    pos = (0, -15, 2),
    fov = 30
)

GLTargetDistantLight(
    pos = (-0.2, -1, 0.5)
)

CCylinder(
    pos = (-3,0,0),
    material = GLMaterial(
                  texture = GLTexture(
                                imagename = "uvmap.png",
                                mode = GL_DECAL
                            )
               )
)

CCylinder(
    pos = (0,0,0),
    material = GLMaterial(
                  diffuse = (1,1,1),
                  texture = GLTexture(
                                imagename = "uvmap.png",
                                mode = GL_MODULATE
                            )
               )
)

CCylinder(
    pos = (3,0,0),
    material = GLMaterial(
                  diffuse = (1,0,0),
                  texture = GLTexture(
                                imagename = "uvmap.png",
                                mode = GL_BLEND,
                                texenvcolor = (1,1,0)
                            )
               )
)



