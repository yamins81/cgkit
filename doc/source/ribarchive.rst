.. % RIBArchive component


:class:`RIBArchive` --- Reference an external RIB archive
=========================================================

The :class:`RIBArchive` class represents geometry that is defined in an external
RIB file. When the scene is rendered using a RenderMan renderer the RIB file
will be included via a call to  :cfunc:`RiReadArchive`. In an interactive
viewer, a :class:`RIBArchive` object will be invisible.


.. class:: RIBArchive(name = "RIBArchive",  filename = None,  auto_insert = True)

   *filename* is the name of the external RIB file.

