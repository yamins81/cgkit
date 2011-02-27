.. % GeomObject


:class:`GeomObject` --- Geometry base class
===========================================

The :class:`GeomObject` class is the base class for all geometries. Instances of
this class are stored in the :attr:`geom` attribute of the world objects and can
be shared among them.


.. class:: GeomObject()

   ...params?...

.. % Methods


.. method:: GeomObject.boundingBox()

   Return the local axis aligned bounding box. The bounding box is given with
   respect to the local transformation L (which is not what you get from the
   transform slot of the world object).


.. method:: GeomObject.drawGL()

   Draw the geometry using OpenGL commands.


.. method:: GeomObject.uniformCount()

   Return the multiplicity of :keyword:`UNIFORM` variables.


.. method:: GeomObject.varyingCount()

   Return the multiplicity of :keyword:`VARYING` variables.


.. method:: GeomObject.vertexCount()

   Return the multiplicity of :keyword:`VERTEX` variables.


.. method:: GeomObject.faceVaryingCount()

   Return the multiplicity of :keyword:`FACEVARYING` variables.


.. method:: GeomObject.faceVertexCount()

   Return the multiplicity of :keyword:`FACEVERTEX` variables.


.. method:: GeomObject.slotSizeConstraint(storage)

   Return a constraint object for primitive variable slots or None if the size of
   the slot should be unconstrained. This method is called when a new primitive
   variable is created. The returned constraint object is used for the array slot
   that holds the values of the variable. *storage* is the storage class of the new
   variable which will be one of ``UNIFORM``, ``VARYING``, ``VERTEX``,
   ``FACEVARYING`` or ``FACEVERTEX``. The method will never be called when
   ``CONSTANT`` or ``USER`` variables are created.


.. method:: GeomObject.newVariable(name, storage, type, multiplicity=1, user_n=0)

   Attaches a new primitive variable to the geometry.

   *storage* specifies the storage class, i.e. how many values are actually stored.
   It must be one of ``CONSTANT``, ``UNIFORM``, ``VARYING``, ``VERTEX``,
   ``FACEVARYING``, ``FACEVERTEX`` or ``USER``. The exact number of values depends
   on the actual geometry. However, ``CONSTANT`` is always exactly one value for
   the entire geometry and ``USER`` is a user defined number specified in *user_n*.

   *type* is the type of the variable and must be one of  ``INT``, ``FLOAT``,
   ``STRING``, ``COLOR``, ``POINT``,  ``VECTOR``, ``NORMAL``, ``MATRIX`` and
   ``HPOINT``.  If *multiplicity* is greater than 1, then an array with that size
   is created.

   Creating a new variable will always create a new slot of that name as well. The
   slot is always an :class:`ArraySlot` (even for ``CONSTANT`` variables). After
   you have created a variable you can use the corresponding slot to manipulate the
   values of the variable.

   Here is an example of a "varying int [3]" variable that's created on a sphere
   geometry. This means, the variable will consist of four 3-tuples of integers
   (one for each parametric corner). ::

      >>> from cgkit.all import *
      >>> sg=SphereGeom()
      >>> sg.newVariable("foo", VARYING, INT, multiplicity=3)
      >>> for v in sg.iterVariables(): print v
      ...
      ('foo', cgkit._core.VarStorage.VARYING, cgkit._core.VarType.INT, 3)
      >>> s=sg.slot("foo")
      >>> s[1]=(1,2,3)
      >>> for f in s: print f
      ...
      (0, 0, 0)
      (1, 2, 3)
      (0, 0, 0)
      (0, 0, 0)


.. method:: GeomObject.deleteVariable(name)

   Delete the primitive variable with the specified name.


.. method:: GeomObject.deleteAllVariables()

   Delete all primitive variables.


.. method:: GeomObject.findVariable(name)

   Search for a particular primitive variable and return its descriptor. ``None``
   is returned if a variable called *name* cannot be found. The return value is a
   4-tuple (name, storage class, type, multiplicity) describing the variable. See
   the :meth:`newVariable` method for a description of the individual elements.


.. method:: GeomObject.iterVariables()

   Return an iterator that iterates over all existing primitive variables. The
   iterator will return the same 4-tuple as returned by  :meth:`findVariable`.


.. method:: GeomObject.convert(targetgeom)

   Convert the geometry into another type of geometry. *targetgeom* is another
   :class:`GeomObject` that will receive the result of the conversion.  If the
   conversion is not possible, a :exc:`NotImplementedError` exception is thrown.

   In the following example, a box geometry is converted into a triangle mesh::

      >>> bg=BoxGeom(segmentsx=3, segmentsy=3, segmentsz=3)
      >>> tm=TriMeshGeom()
      >>> bg.convert(tm)
      >>> print len(tm.verts)
      56
      >>> print len(tm.faces)
      108

