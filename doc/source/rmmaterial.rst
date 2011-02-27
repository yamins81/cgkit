.. % RMMaterial


:class:`RMMaterial` --- RenderMan material
==========================================

The :class:`RMMaterial` class is a special material that is only of use if you
are creating images via a RenderMan renderer and you want to write your shaders
in external shader files or use shaders that are already compiled.

The material may consist of a surface shader, a displacement shader and an
interior shader. The shader source files (or only the shader names) are passed
via :class:`RMShader` instances as arguments to the constructor. If the
:class:`RMShader` instance points to a file, the material object will take care
of the compilation of the file. Otherwise, it is up to you to compile the shader
and make sure that the renderer can find it.


.. class:: RMMaterial(name = "RMMaterial",  surface = None, displacement = None, displacementbound = ("current", 0.0), interior = None, color = None, opacity = None )

   *surface* specifies the surface shader to use. It can either be a string
   containing the shader name or a :class:`RMShader` instance representing the
   shader. You can also pass ``None`` if no surface shader should be instantiated.

   *displacment* specifies the displacment shader to use. It can either be a string
   containing the shader name or a :class:`RMShader` instance representing the
   shader. You can also pass ``None`` if no displacment shader should be
   instantiated.

   *displacementbound* is a tuple (coordinate system, distance) that specifies the
   maximum displacement. The distance is the maximum amount that a surface point is
   displaced and  is given in the specified coordinate  system.

   *interior* specifies the interior shader to use. It can either be a string
   containing the shader name or a :class:`RMShader` instance representing the
   shader. You can also pass ``None`` if no interior shader should be instantiated.

   *color* is the color that should be set via :cfunc:`RiColor`.

   *opacity* is the opacity that should be set via :cfunc:`RiOpacity`.

The parameters of the shaders are made available as attributes of the material
objects. The corresponding slots can be obtained by adding the suffix ``_slot``
to the name. Attribute names in the surface shader have priority over the
attributes in the displacement shader which in turn has priority over the
interior shader. This means, if there are identical parameter names in all
shaders you will access the parameter of the surface shader. You can also access
the attributes of each shader via the *surface*, *displacement* and *interior*
attributes which contain the corresponding :class:`RMShader` instances.

Example::

   mat = RMMaterial(surface = RMShader("mysurface.sl"),
                    displacement = RMShader("c:\\shaders\\dented.sl"),
                    color = (1,0.5,0.8)
                    )
   ...
   Sphere(pos=(1,2,3), radius=0.5, material=mat)

In this example, the material uses the surface shader :file:`mysurface.sl` and
the displacement shader :file:`c:\\shaders\\dented.sl`. The shaders will be
compiled automatically because the shader source files are given (instead of
just the shader names).

.. % ------------------------------------------------------------


.. _rmshader:

:class:`RMShader` ---  RenderMan shader
---------------------------------------

The :class:`RMShader` class encapsulates a single RenderMan shader.


.. class:: RMShader(shader = None,  transform = mat4(1),  cpp = None,  cpperrstream = sys.stderr,  includedirs = None, defines = None, params = None,  paramlist)

   *shader* is either the name of a shader or the shader source file. If a shader
   file is given then the shader is read to extract the parameters. Each parameter
   will be made available as slot.

   *transform* is a :class:`mat4` containing a transformation that should be
   applied to the shader. This means you can transform the shader relative to the
   object it is applied to.

   *cpp* determines the preprocessor that should be used when extracting
   parameters. *cpperrstream* is used to output errors from the preprocessor.
   *includedirs* is a list of strings that contain directories where to look for
   include files. *defines* is a list of tuples (name, value) that specify the
   predefined symbols to use (see the function :func:`slparams.slparams` (section
   :ref:`slparams`) for details).

   *params* can be used to declare parameters if the shader source is not
   available. The value must be a dictionary that contains token/value pairs. The
   token may contain an inline declaration.

   Any additional keyword argument is also considered to be a shader parameter.
   However, this parameter cannot have an inline declaration, so it is recommended
   to declare the parameter afterwards using the :meth:`declare` method, otherwise
   no declaration will be  written in the RIB file and you have to care about the
   declaration yourself.

.. % shaderName


.. method:: RMShader.shaderName()

   Return the shader name or ``None`` if the name is not known.

.. % shaderType


.. method:: RMShader.shaderType()

   Return the shader type as a string (``"surface"``, ``"displacement"``,
   ``"light"``, ...) or ``None`` if the type is not known.

.. % declare


.. method:: RMShader.declare(name, type=None, cls=None, arraysize=None, default=None)

   Declare a shader parameter. *name* is the parameter name. *name* may also
   contain the entire declaration in SL syntax. In this case, all other arguments
   are ignored, otherwise they provide the missing information. *type* is the only
   parameter that is mandatory if name does not contain the entire declaration. It
   contains the name of the SL parameter type (float, string, color, point, vector,
   normal, matrix). *cls* is the storage class (uniform, varying). *arraysize*
   specifies the size of the array and *default* contains the default value.

   When a parameter is declared it is added to the list of known parameters and a
   corresponding slot (``<name>_slot``) is created.

   Examples::

      shader.declare('uniform float Ka=0.5')
      shader.declare('uniform float Ka')
      shader.declare('float Ka')
      shader.declare('Ka', type='float')

   A parameter that was specified in the constructor is used as default value when
   the parameter is declared. In this case, any default value passed to the
   :func:`declare` method is ignored.

.. % params


.. method:: RMShader.params()

   Return a dictionary containing the parameters for the current time. The key is
   the parameter name (containing an inline declaration if available) and the value
   is the current value of the parameter.

