
:mod:`objmtl` --- Reading Wavefront OBJ/MTL files
=================================================

.. module:: cgkit.objmtl
   :synopsis: Reading Wavefront OBJ/MTL files


This module contains the :class:`OBJReader` and :class:`MTLReader` classes which
can be used as a base class for reading Wavefront OBJ and MTL files. The classes
read the files and invoke callback methods with the corresponding data in the
file. Derived classes have to implement those callback methods and process the
data as appropriate.


OBJReader class
---------------

The :class:`OBJReader` class reads Wavefront OBJ files and calls appropriate
methods which have to be implemented in a derived class.


.. class:: OBJReader()

   Creates an instance of the reader.


.. method:: OBJReader.read(f)

   Read the content of a file. *f* must be a file like object that can be used to
   read the content of the file.


.. method:: OBJReader.begin()

   Callback method that is called before the file is read.


.. method:: OBJReader.end()

   Callback method that is called after the file was read.


.. method:: OBJReader.handleUnknown(cmd, arglist)

   This method is called when a keyword is encountered that has no  corresponding
   handler method.

The following are the predefined handler methods:


.. method:: OBJReader.call(filename, *args)


.. method:: OBJReader.csh(cmd)


.. method:: OBJReader.mtllib(*files)


.. method:: OBJReader.usemtl(name)


.. method:: OBJReader.g(*groups)


.. method:: OBJReader.s(groupnumber)


.. method:: OBJReader.v(vert)


.. method:: OBJReader.vp(vert)


.. method:: OBJReader.vn(normal)


.. method:: OBJReader.vt(tvert)


.. method:: OBJReader.f(*verts)


.. method:: OBJReader.o(name)


.. method:: OBJReader.bevel(onoff)


.. method:: OBJReader.c_interp(onoff)


.. method:: OBJReader.d_interp(onoff)


.. method:: OBJReader.lod(level)


.. method:: OBJReader.shadow_obj(filename)


.. method:: OBJReader.trace_obj(filename)

.. % ----------------------------------------------------------------


MTLReader class
---------------


.. class:: MTLReader()


   .. method:: MTLReader.read(f)

      Read the content of a file. *f* must be a file like object that can be used to
      read the content of the file.


   .. method:: MTLReader.begin()

      Callback method that is called before the file is read.


   .. method:: MTLReader.end()

      Callback method that is called after the file was read.


   .. method:: MTLReader.handleUnknown(cmd, arglist)

      This method is called when a keyword is encountered that has no  corresponding
      handler method.

   The following are the predefined handler methods:


   .. method:: MTLReader.newmtl(name)

      Start of a new material definition.


   .. method:: MTLReader.illum(model)

      Illumination model.


   .. method:: MTLReader.Ka(col)

      Ambient color.


   .. method:: MTLReader.Kd(col)

      Diffuse color.


   .. method:: MTLReader.Ks(col)

      Specular color.


   .. method:: MTLReader.Ke(col)

      Emissive color.


   .. method:: MTLReader.Ns(shininess)


   .. method:: MTLReader.Ni(ref)

      Refraction index.


   .. method:: MTLReader.Tr(alpha)

      Transparency.


   .. method:: MTLReader.d(alpha)

      Transparency.


   .. method:: MTLReader.Tf(col)

      Transparency.


   .. method:: MTLReader.sharpness(v)


   .. method:: MTLReader.map_Ka(mapname, options)


   .. method:: MTLReader.map_Kd(mapname, options)


   .. method:: MTLReader.map_Ks(mapname, options)


   .. method:: MTLReader.map_Ke(mapname, options)


   .. method:: MTLReader.map_Ns(mapname, options)


   .. method:: MTLReader.map_d(mapname, options)


   .. method:: MTLReader.map_Bump(mapname, options)


   .. method:: MTLReader.refl(mapname, options)

.. % ----------------------------------------------------------------


Handling new keywords
---------------------

The :class:`OBJReader` and :class:`MTLReader` classes can also handle keywords
that are not known to the classes. For each keyword, the classes look for a
method called ``handle_<keyword>`` which is invoked with the arguments as
parameters. If such a handler is not found, the :meth:`handleUnknown` method is
called.

For the standard keywords such ``handle_<keyword>`` handlers are already
available. Their task is to preprocess the arguments and call the final handler
method (which is just named after the keyword).

