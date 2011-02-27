.. % SpotLight3DS


:class:`RMLightSource` --- RenderMan light source
=================================================


.. class:: RMLightSource(name = "RMLightSource",  shader = None )

   *shader* specifies the light shader that should be used. It can either be a
   string containing the shader name or a :class:`RMShader` instance representing
   the shader (see section :ref:`rmshader`).

The parameters of the shader is made available as attributes of the light source
object. The corresponding slots can be obtained by adding the suffix ``_slot``
to the name.

