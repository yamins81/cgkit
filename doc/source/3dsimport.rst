.. % 3DS import


3D Studio 3DS import
--------------------

The 3DS import plugin reads 3D Studio 3DS files [#]_. All light sources will be
imported as :class:`SpotLight3DS` objects and all materials as
:class:`Material3DS` objects. These objects approximate the appearance of the
light source/material when rendered with RenderMan but are not supported by the
interactive viewer. This means you have to replace the materials and the light
sources by appropriate objects if you want to display the scene interactively.

.. note::

   Animations are not supported yet. The content of the file will be evaluated at
   the current time (i.e. the time set in cgkit, not the time set in the 3DS file).

The plugin supports the following options that can be passed to the :func:`load`
command:

+------------+-------------------+------------------------------+
| Option     | Default           | Description                  |
+============+===================+==============================+
| ``flags``  | ``GEOM_INIT_ALL`` | Flags for the mesh creation. |
+------------+-------------------+------------------------------+
| ``parent`` | ``None``          | Parent object to be used for |
|            |                   | the entire scene.            |
+------------+-------------------+------------------------------+

The *flags* option specifies what data will be attached to the generated meshes.
It can be a combination of the following flags:

+-------------------------+-------------------------------------------+
| Flag                    | Description                               |
+=========================+===========================================+
| ``GEOM_INIT_NORMALS``   | Compute normals                           |
+-------------------------+-------------------------------------------+
| ``GEOM_INIT_MATIDS``    | Add material IDs                          |
+-------------------------+-------------------------------------------+
| ``GEOM_INIT_TEXELS``    | Add texture coordinates                   |
+-------------------------+-------------------------------------------+
| ``GEOM_INIT_FLAGS``     | Add the face flags (e.g. edge visibility) |
+-------------------------+-------------------------------------------+
| ``GEOM_INIT_SMOOTHING`` | Add smoothing group information           |
+-------------------------+-------------------------------------------+
| ``GEOM_INIT_ALL``       | All of the above                          |
+-------------------------+-------------------------------------------+

.. note::

   The plugin uses the lib3ds library which is available at
   `<http://lib3ds.sourceforge.net>`_.

     | *The 3D Studio File Format Library*
     | *Copyright (C) 1996-2001 by J.E. Hoffmann*
     | *All rights reserved.*

.. rubric:: Footnotes

.. [#] Note that the 3DS format is the format that the old 3D Studio (for DOS) was
   using, it is not the native format of 3D Studio MAX anymore.

