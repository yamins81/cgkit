.. % RIBExport


.. _ribexport:

RenderMan RIB export
--------------------

The module :mod:`cgkit.ribexport` contains the class :class:`RIBExporter` that
exports the current scene as RIB and creates shaders from the materials and
light sources used in the scene.

To use the exporter you can simply use the :func:`save()<cgkit.cmds.save>`
command and pass a file name with suffix ``.rib``. The plugin supports the
following options that can be passed to the :func:`save()<cgkit.cmds.save>` command:

+------------------------+-----------+------------------------------+
| Option                 | Default   | Description                  |
+========================+===========+==============================+
| ``camera``             | ``None``  | Camera object to be used     |
+------------------------+-----------+------------------------------+
| ``output``             | ``None``  | Output image file name or    |
|                        |           | output specs                 |
+------------------------+-----------+------------------------------+
| ``output_framebuffer`` | ``True``  | Framebuffer output?          |
+------------------------+-----------+------------------------------+
| ``bake``               | ``False`` | Activate texture baking mode |
+------------------------+-----------+------------------------------+
| ``bakemodel``          | ``None``  | Determines the model to bake |
+------------------------+-----------+------------------------------+
| ``bakestvar``          | ``"st"``  | Variable name of the bake    |
|                        |           | texture coordinates          |
+------------------------+-----------+------------------------------+

*camera* specifies the camera object to be used for rendering the scene. If
``None`` is specified the first camera found in the scene is used.

*output* is the output image file name or a list of output specifiers. Each
specifier is a 4-tuple (*filename*, *type*, *mode*, *params*) containing the
parameters for a :func:`RiDisplay` call. The additional parameters must be given
as a dictionary. *output* can also be ``None`` in which case no
:func:`RiDisplay` call is made.

*output_framebuffer* is a flag that specifies if a framebuffer display should be
opened in addition to writing the output image file to disk. This flag is only
used when *output* is a string.

If *bake* is ``True`` the exported scene will bake a texture map instead of
producing a final image.

*bakemodel* is either the name of a WorldObject or a WorldObject itself. The
texture will be baked for the specified model. This parameter doesn't have to be
specified if there is only one mesh in the scene.

*bakestvar* is the name of the primitive variable that holds the  texture
coordinates that should be used for baking.

The plugin uses the following global options:

+------------------+---------------+--------------------------------+
| Option           | Default       | Description                    |
+==================+===============+================================+
| ``displaymode``  | ``"rgb"``     | Display mode                   |
+------------------+---------------+--------------------------------+
| ``displaytype``  | ``"file"``    | Display type                   |
+------------------+---------------+--------------------------------+
| ``output``       | ``None``      | Output image file name or      |
|                  |               | output specs (see above)       |
+------------------+---------------+--------------------------------+
| ``resolution``   | ``(640,480)`` | Output image resolution        |
+------------------+---------------+--------------------------------+
| ``pixelsamples`` | ``(2,2)``     | Number of pixel samples        |
+------------------+---------------+--------------------------------+
| ``shadingrate``  | ``1.0``       | Shading rate                   |
+------------------+---------------+--------------------------------+
| ``pixelfilter``  | ``None``      | Pixel filter setting           |
+------------------+---------------+--------------------------------+
| ``tiles``        | ``None``      | Render the image in tiles      |
+------------------+---------------+--------------------------------+
| ``rib``          | ``None``      | Additional global RIB requests |
+------------------+---------------+--------------------------------+

*displaymode* is the mode string for the :func:`RiDisplay` call and determines
what data is written to the output. Usually you might want to switch between
"rgb" (default) and "rgba".

*output* is the image output name or a list of output specifiers  (see above).

*resolution* is either a 2-tuple (*width*, *height*) or a 3-tuple  (*width*,
*height*, *pixel aspect*) specifying the outpout image resolution.

*pixelsamples* is a 2-tuple containing the pixel samples setting.

*shadingrate* contains the shading rate setting.

*pixelfilter* is a 2-tuple (*filter*, (*xwidth*, *ywidth*)) that sets the pixel
filter to use. If ``None`` is passed the default pixel filter of the renderer is
used. Example: ("gaussian", (2,2))

*tiles* contains two sequences that defines the positions where the image is
split. The first sequence contains the x positions and the second sequence the y
positions where a split should occur. Each value lies between 0 and 1, so for
example tiles=((0.5,), (0.5,)) will render the image in four equal tiles. The
tiles can than be stitched together using the :mod:`stitch` module.

*rib* is a string containing additional RIB requests that are written in front
of the frames.

It is possible to attach user RIB requests to an object simply by adding a
string attribute called ``rib``. When present this string will be written right
before the geometry calls. For example, this can be used to set attributes that
are specific to a particular renderer::

   s = Sphere(...)
   s.rib = 'Attribute "visibility" "transmission" "opaque"'

For an object to be exported as RIB it has to use a geometry that supports the
:class:`IGeometry` protocol. Materials must support the :class:`IMaterial`
protocol and light sources the :class:`ILightSource` protocol. For details on
these protocols see the sub sections below. If there is an object that does not
support one of these protocols an adapter can be written that implements the
protocol on behalf of the original object.

The remainder of this section is meant to be read by developers who want to
extend the functionality of the exporter either by implementing  adapter classes
for existing classes or by implementing new geometries, materials or light
sources that natively support the respective protocol.

.. % --------------------------------------------


The :class:`IGeometry` protocol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Every :class:`GeomObject` that supports the :class:`IGeometry` protocol can be
exported as RIB. If the geometry does not support the protocol it will be
ignored.

The :class:`IGeometry` protocol only specifies the presence of one method:


.. method:: IGeometry.render(matid)

   Creates Ri geometry requests for the geometry that has the material with  id
   *matid* assigned to it.  The geometry should be created in the local coordinate
   system of the :class:`GeomObject`. The primitive variables should also be
   exported. The method can assume that there is already an enclosing
   :func:`RiAttributeBegin`/:func:`RiAttributeEnd` block around the  call.

Here is an example of an adapter class that implements the :class:`IGeometry`
protocol for the :class:`SphereGeom` (which knows nothing about RenderMan)::

   import protocols
   from ri import *

   # Adapter class that implements the IGeometry protocol on behalf of the SphereGeom class
   class SphereAdapter:

       protocols.advise(instancesProvide=[IGeometry], asAdapterForTypes=[SphereGeom])

       def __init__(self, spheregeom, proto):
           self.geom = spheregeom

       def render(self, matid):
           # A sphere can only have one single material
   	if matid==0:
               r = self.geom.radius
               RiSphere(r, -r, r, 360)

.. % --------------------------------------------


The :class:`IMaterial` protocol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A material that supports the :class:`IMaterial` protocol will be mapped to a
surface shader, displacement shader and interior shader. To support the
:class:`IMaterial` protocol the following methods have to be implemented:


.. method:: IMaterial.createPasses()

   Returns a list of :class:`RenderPass` objects necessary for this material
   instance. These passes may be used to create environment maps, for example. If
   no extra passes are required an empty list has to be returned.


.. method:: IMaterial.preProcess(exporter)

   This method is called before the image is rendered and can be used to create or
   copy image maps or do other initializations that have to be done before the
   actual rendering starts. The exporter instance is provided as argument to the
   method (for example, to find out where image maps are stored).


.. method:: IMaterial.color()

   Return the color (as a 3-sequence of floats) for the :func:`RiColor` call. If no
   color is required the method may return ``None`` in which case no ``Color`` call
   is made.


.. method:: IMaterial.opacity()

   Return the opacity (as a 3-sequence of floats) for the :func:`RiOpacity` call.
   If no opacity is required the method may return ``None`` in which case no
   ``Opacity`` call is made.


.. method:: IMaterial.surfaceShaderName()

   Return the name of the surface shader for this material. The exporter may still
   modify this name to make it unique among all generated shaders.


.. method:: IMaterial.surfaceShaderSource()

   Return the RenderMan Shading Language source code for the surface shader.
   Instead of the shader name the generated source code should contain the variable
   ``$SHADERNAME`` that will be substituted with the actual name of the shader. The
   method may also return ``None`` if no shader should be created. In this case,
   the name returned by :meth:`surfaceShaderName` is assumed to be the name of an
   existing shader.


.. method:: IMaterial.surfaceShaderParams(passes)

   Returns a dictionary that contains the shader parameters that should be used for
   the surface shader. The key is the name of the parameter (including inline
   declarations) and the value is the actual parameter value at the current time.
   The *passes* argument contains the list of passes as generated by
   :meth:`createPasses`. This list can be used to obtain the actual name of an
   environment map, for example.


.. method:: IMaterial.surfaceShaderTransform()

   Return a mat4 containing the transformation that should be applied to the
   shader.


.. method:: IMaterial.displacementShaderName()

   Return the name of the displacement shader for this material. The exporter may
   still modify this name to make it unique among all generated shaders. You can
   also return ``None`` if no displacement shader is required.


.. method:: IMaterial.displacementShaderSource()

   Return the RenderMan Shading Language source code for the displacement shader.
   Instead of the shader name the generated source code should contain the variable
   ``$SHADERNAME`` that will be substituted with the actual name of the shader. The
   method may also return ``None`` if no shader should be created. In this case,
   the name returned by :meth:`displacementShaderName` is assumed to be the name of
   an existing  shader.


.. method:: IMaterial.displacementShaderParams(passes)

   Returns a dictionary that contains the shader parameters that should be used for
   the displacement shader. The key is the name of the parameter (including inline
   declarations) and the value is the actual parameter value at the current time.
   The *passes* argument contains the list of passes as generated by
   :meth:`createPasses`.


.. method:: IMaterial.displacementShaderTransform()

   Return a mat4 containing the transformation that should be applied to the
   shader.


.. method:: IMaterial.displacementBound()

   Returns a tuple (*coordinate system*, *distance*) that specifies the maximum
   displacement. The distance is the maximum amount that a surface point is
   displaced and is given in the specified coordinate system.


.. method:: IMaterial.interiorShaderName()

   Return the name of the interior shader for this material. The exporter may still
   modify this name to make it unique among all generated shaders. You can also
   return ``None`` if no interior shader is required.


.. method:: IMaterial.interiorShaderSource()

   Return the RenderMan Shading Language source code for the interior shader.
   Instead of the shader name the generated source code should contain the variable
   ``$SHADERNAME`` that will be substituted with the actual name of the shader. The
   method may also return ``None`` if no shader should be created. In this case,
   the name returned by :meth:`displacementShaderName` is assumed to be the name of
   an existing  shader.


.. method:: IMaterial.interiorShaderParams(passes)

   Returns a dictionary that contains the shader parameters that should be used for
   the interior shader. The key is the name of the parameter (including inline
   declarations) and the value is the actual parameter value at the current time.
   The *passes* argument contains the list of passes as generated by
   :meth:`createPasses`.


.. method:: IMaterial.interiorShaderTransform()

   Return a mat4 containing the transformation that should be applied to the
   shader.

.. % --------------------------------------------


The :class:`ILightSource` protocol
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Every world object that supports the :class:`ILightSource` protocol will be used
as light source to illuminate the scene. In order to  support the
:class:`ILightSource` protocol the following four methods have to be
implemented:


.. method:: ILightSource.createPasses()

   Returns a list of :class:`RenderPass` objects necessary for this light source
   instance. Usually the light sources will make use of the :class:`ShadowPass`
   pass. If no extra passes are required an empty list has to be returned.


.. method:: ILightSource.shaderName()

   Return the name of the light source shader for this light source. The exporter
   may still modify this name to make it unique among all generated shaders.


.. method:: ILightSource.shaderSource()

   Return the RenderMan Shading Language source code for this light source. Instead
   of the shader name the generated source code should contain the variable
   ``$SHADERNAME`` that will be substituted with the actual name of the shader. The
   method may also return ``None`` if no shader should be created. In this case,
   the name returned by :meth:`shaderName` is assumed to be the name of an existing
   shader.


.. method:: ILightSource.shaderParams(passes)

   Returns a dictionary that contains the shader parameters that should be used for
   this light source instance. The key is the name of the parameter (including
   inline declarations) and the value is the actual parameter value at the current
   time. The *passes* argument contains the list of passes as generated by
   :meth:`createPasses`. This list can be used to obtain the actual name of a
   shadow map, for example.

.. % --------------------------------------------


:class:`RenderPass` --- Base class for all passes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The creation of an image may take several render passes where shadow maps,
environment maps and eventually the final image are created. The number  of
passes is dependent on the number and types of light sources and the  materials
used in the scene. Every render pass is represented by a class that is derived
from the :class:`RenderPass` class.

An actual pass has to implement the :meth:`doPass` method where it  has to
output a frame block that generates one or more output files. To do the scene
export it can use the methods from the exporter.


.. class:: RenderPass(output, owner=None)

   *output* is a list of tuples (*name*, *type*, *mode*,  *params*) that each
   defines the output to create in this pass.  Each tuple contains the parameters
   necessary for a ``RiDisplay()`` call. *name* is the name of the output file,
   *type* is the output type (such as "file" or "zfile"), *mode* specifies what
   information will be stored in the file (``RI_RGB``, ``RI_RGBA``, ...) and
   *params* is a dictionary with extra parameters.

   Every :class:`RenderPass` instance has an attribute *exporter* that will contain
   a reference to the exporter. So the pass object may use the exporter methods to
   output the scene.


.. method:: RenderPass.done()

   Check if this pass is already done or not. This method is used when shader
   parameters have to be determined. The output of a pass may only be used once the
   pass is done and the output really exists (for example, you cannot use a shadow
   map until it was created).


.. method:: RenderPass.doPass(framenr)

   This method has to be overwritten in derived classes where the actual frame
   block is generated. *framenr* is the frame number to use for the
   :func:`RiFrameBegin` call.


.. method:: RenderPass.realFilename(filename)

   Translate the logical file name *filename* into a real file name. A
   :exc:`ValueError` exception is generated if *filename* is not a logical file
   name of this pass.


.. method:: RenderPass.getFilenameTable()

   Return the filename translation table. The return value is a dictionary that
   maps logical file names to real file names.


.. method:: RenderPass.setFilenameTable(tab)

   Set an updated filename table. This method is used to set the updated file name
   table where no name clashes occur with other passes.


.. method:: RenderPass.initDisplays()

   This method calls :func:`RiDisplay` for all specified outputs. This is a helper
   method that can be used in :meth:`doPass`.

.. % --------------------------------------------


:class:`ImagePass` --- Creates the final image pass
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


.. class:: ImagePass(output, cam)

.. % --------------------------------------------


:class:`ShadowPass` --- Creates a shadow map
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


.. class:: ShadowPass(output, light, fov, resolution)

.. % --------------------------------------------


:class:`RIBExporter` --- The exporter class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Besides the usual methods that every exporter must implement this exporter has
the following helper methods that have to be used by :class:`RenderPass`
objects:


.. method:: RIBExporter.isExportable(wobj)


.. method:: RIBExporter.applyViewTransform(V)

   Applies the view transformation V (given as a ``mat4``). This method corresponds
   to a :func:`RiConcatTransform` call. It outputs the view transformation  *V*
   (which transforms from world to camera coordinates) as a RenderMan
   transformation. The handedness of the scene is taken into account.


.. method:: RIBExporter.applyLightSource(lgt)

   Apply the light source *lgt* which must be a light source world object. This
   method corresponds to a :func:`RiLightSource` call.


.. method:: RIBExporter.applyTransformation(T, linearvel=None, angularvel=None)

   Apply the transformation *T* (given as a ``mat4``). This method corresponds to a
   :func:`RiConcatTransform` call.  It outputs *T* as a RenderMan transformation.
   If a linear or angular  velocity is given an motion block is written to enable
   motion blur.


.. method:: RIBExporter.applyMaterial(mat)

   Apply the material *mat* which must be a :class:`Material` object. This method
   corresponds to the calls :func:`RiColor`, :func:`RiOpacity`, :func:`RiSurface`,
   :func:`RiDisplacement` and :func:`RiInterior`.


.. method:: RIBExporter.applyGeometry(geom)

   Apply the geometry *geom* which must be a :class:`GeomObject`.


.. method:: RIBExporter.writeShader(name, source)

   Write a shader source file and return the name of the shader. Usually,  you
   don't have to call this method yourself as it is called by the above
   applyXyz()-methods.

