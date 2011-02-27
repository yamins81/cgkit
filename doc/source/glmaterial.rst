.. % GLMaterial


:class:`GLMaterial` --- OpenGL material
=======================================

The :class:`GLMaterial` class represents the material model of the standard
OpenGL API. All colors are represented as (r,g,b,a). If you don't use blending
you can leave out the alpha value in the constructor.


.. class:: GLMaterial(name = "GLMaterial",  ambient = (0.2, 0.2, 0.2, 1.0), diffuse = (0.7, 0.7, 0.7, 1.0), specular = (0, 0, 0, 1), shininess = 0.0, emission = (0, 0, 0, 1), blend_factors = None, texture = None, vertex_shader = None, fragment_shader = None, density = 1.0)

   *ambient* is the ambient color of the material.

   *diffuse* is the diffuse color of the material.

   *specular* is the color of the specular highlight.

   *shininess* determines the size of the highlight and lies  between 0.0 and
   128.0. The higher the value, the smaller and brighter the highlight.

   The *emission* color can be used to simulate objects that emit light.

   *blend_factors* is a 2-tuple with the parameters for the  :cfunc:`glBlendFunc`
   call which indicate how to compute the source and destination blend factors
   (see `glBlendFunc()
   <http://pyopengl.sourceforge.net/documentation/manual/glBlendFunc.3G.html>`_).
   Blending is disabled if *blend_factors* is ``None``.

   *texture* is either ``None``, a single :class:`GLTexture` object or a list of
   :class:`GLTexture` objects specifying the texture image(s) to use.

   *vertex_shader* is either ``None``, a single :class:`GLShader` object or a list
   of :class:`GLShader` objects specifying the vertex shaders to use.

   *fragment_shader* is either ``None``, a single :class:`GLShader` object or a
   list of :class:`GLShader` objects specifying the fragment shaders to use.

   *density* is the density value used for physical simulations.

.. % Methods


.. method:: GLMaterial.getNumTextures()

   Return the current size of the texture array.


.. method:: GLMaterial.setNumTextures(num)

   Set a new size for the texture array.


.. method:: GLMaterial.getTexture(idx=0)

   Get a stored texture. The method returns ``None`` if the given index is out of
   range or if there is no texture stored at that position.


.. method:: GLMaterial.setTexture(tex, idx=0)

   Set a new texture. An :exc:`IndexError` exception is thrown if the  index is out
   of range.


.. method:: GLMaterial.getNumVertexShaders()

   Return the current size of the vertex shader array.


.. method:: GLMaterial.setNumVertexShaders(num)

   Set a new size for the vertex shader array.


.. method:: GLMaterial.getVertexShader(idx=0)

   Get a vertex shader object. The method returns ``None`` if the given index is
   out of range or if there is no shader object stored at that position.


.. method:: GLMaterial.setVertexShader(shader, idx=0)

   Set a new vertex shader object. An :exc:`IndexError` exception is  thrown if the
   index is out of range. A ValueError exception is thrown if the  shader is not of
   type ``VERTEX``.


.. method:: GLMaterial.getNumFragmentShaders()

   Return the current size of the fragment shader array.


.. method:: GLMaterial.setNumFragmentShaders(num)

   Set a new size for the fragment shader array.


.. method:: GLMaterial.getFragmentShader(idx=0)

   Get a fragment shader object. The method returns ``None`` if the given index is
   out of range or if there is no shader object stored at that position.


.. method:: GLMaterial.setFragmentShader(shader, idx=0)

   Set a new fragment shader object. An :exc:`IndexError` exception is  thrown if
   the index is out of range. A ValueError exception is thrown if the  shader is
   not of type ``FRAGMENT``.

.. % -----------------------


:class:`GLTexture` --- Specifying a texture map
-----------------------------------------------

The :class:`GLTexture` class is used to describe the parameters of an OpenGL
texture map for use with the :class:`GLMaterial` class.


.. class:: GLTexture(imagename = "", image = None, mode = GL_DECAL, mipmap = True, mag_filter = GL_LINEAR, min_filter = GL_LINEAR, wrap_s = GL_REPEAT, wrap_t = GL_REPEAT, internalformat = GL_RGB, texenvcolor = vec4(1), transform = mat4(1), size = None,  environment_map = False )

   *imagename* is the name of the image file that should be used as a texture map.
   If the image resolution is not a power of 2, the image is scaled up to the next
   higher power of 2 resolution.

   It is also possible to pass the actual image data in the *image* parameter.  The
   image can either be a PIL image or the raw RGB data. In the latter case, you
   must explicitly specify the image resolution (which must then be a power of 2
   resolution) in the *size* argument. The *imagename* argument is ignored if the
   data is passed via the *image* argument.

   *mode* specifies how the image is applied to the object. It can be one of
   ``GL_REPLACE``, ``GL_MODULATE``, ``GL_DECAL`` and  ``GL_BLEND``. In the latter
   case, the blend color is given by *texenvcolor*  (see `glTexEnv()
   <http://pyopengl.sourceforge.net/documentation/manual/glTexEnv.3G.html>`_).

   *mipmap* determines whether mip mapping should be used or not.

   *mag_filter* is the filter to use when magnification occurs (i.e. when the
   texture appears larger on screen that it actually is). It can be either
   ``GL_NEAREST`` or ``GL_LINEAR``.

   *min_filter* is the filter to use when minification occurs (i.e. when the
   texture appears smaller on screen that it actually is). It can be one of
   following values (see `glTexParameter()
   <http://pyopengl.sourceforge.net/documentation/manual/glTexParameter.3G.html>`_):

   .. % \code{GL_NEAREST}, \code{GL_LINEAR}, \code{GL_NEAREST_MIPMAP_NEAREST},
   .. % \code{GL_NEAREST_MIPMAP_LINEAR}, \code{GL_LINEAR_MIPMAP_NEAREST} or
   .. % \code{GL_LINEAR_MIPMAP_LINEAR}

   * ``GL_NEAREST``
   * ``GL_LINEAR``
   * ``GL_NEAREST_MIPMAP_NEAREST``
   * ``GL_NEAREST_MIPMAP_LINEAR``
   * ``GL_LINEAR_MIPMAP_NEAREST``
   * ``GL_LINEAR_MIPMAP_LINEAR``

   If ``GL_LINEAR`` is specified and mip mapping is used then the filter is
   automatically set to ``GL_LINEAR_MIPMAP_LINEAR``

   *wrap_s* and *wrap_t* specify what happens if the texture coordinate leave the
   range 0-1. They can be one of ``GL_REPEAT``, ``GL_CLAMP`` and
   ``GL_CLAMP_TO_EDGE`` (see `glTexParameter()
   <http://pyopengl.sourceforge.net/documentation/manual/glTexParameter.3G.html>`_).

   *internalformat* specifies how the image data will be stored in memory. Usually,
   you'll either specify ``GL_RGB`` or ``GL_RGBA`` if you have alpha values in your
   image and you want to use them (see `glTexImage2D()
   <http://pyopengl.sourceforge.net/documentation/manual/glTexImage2D.3G.html>`_).

   *transform* is a transformation that is applied to the texture coordinates.

   *size* is a 2-tuple containing the desired texture map resolution which must be
   a power of 2. The image is resized to the specified resolution. If *size* is
   ``None`` then the next higher power of 2 value is used.

   If *environment_map* is ``True`` the image is used as a  latitude/longitude
   environment map.

.. % -----------------------


:class:`GLShader` --- Specifying a shader
-----------------------------------------

The :class:`GLShader` class is used to add a OpenGL 2 shader source file to a
:class:`GLMaterial` class.


.. class:: GLShader(shadertype, filename,  cpp = None, cpperrstream = sys.stderr, **shaderparams )

   *shadertype* specifies whether this shader is vertex shader or a fragment
   shader. The value can either be ``GLShader.ShaderType.VERTEX`` (or
   ``GLSLANG_VERTEX``) or ``GLShader.ShaderType.FRAGMENT`` (or
   ``GLSLANG_FRAGMENT``).

   *filename* is the shader source file name.

   *cpp* determines the preprocessor that should be used when extracting shader
   parameters. *cpperrstream* is used to output errors from the preprocessor (see
   the function :func:`glslangparams.glslangparams`  (section :ref:`glslangparams`)
   for details).

   Any additional keyword argument is assumed to be a shader parameter.

