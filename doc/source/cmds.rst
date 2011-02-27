:mod:`cmds` --- Scene-specific commands
=======================================

.. module:: cgkit.cmds
   :synopsis: Scene-specific commands

This chapter describes the builtin commands which are implemented in the
:mod:`cgkit.cmds` module and which are always available. New commands can be
added via the :func:`register` function (for example, by plugins), so there
might actually be more commands available than are listed here.

.. % cmds

.. % fitCone


.. function:: fitCone(pos, obj)

   Compute a cone that has its apex at *pos* and that includes *obj*. The generated
   cone is the minimal cone that entirely contains the bounding box of *obj* (which
   must be a :class:`WorldObject`). *pos* is the apex of the cone given in world
   coordinates.  The return value is a tuple (*n*, *w*) where *n* is the  axis
   direction of the cone and *w* is the (full) angle in radians.

.. % convertToTriMesh


.. function:: convertToTriMesh(objs)

   Convert all specified objects into triangle meshes. *obj* may be an individual
   world object or a sequence of objects. Each object may be specified by name or
   by reference.

.. % setupObjectNames


.. function:: setupObjectNames()

   Return a string that can be executed to 'import' all scene names. After
   executing the returned string you can access all top level objects via their
   name.

   Example::

      >>> listWorld()
      Root
      +---Bottom (Box/BoxGeom)
      +---GLPointLight (GLPointLight/-)
      +---GLPointLight1 (GLPointLight/-)
      +---Middle (Sphere/SphereGeom)
      +---TargetCamera (TargetCamera/-)
      +---Top (Box/BoxGeom)
      >>> exec setupObjectNames()
      >>> Bottom
      <cgkit.box.Box object at 0x094765D0>
      >>> TargetCamera
      <cgkit.targetcamera.TargetCamera object at 0x09470BD0>
      >>> Top
      <cgkit.box.Box object at 0x09476690>

.. % group()


.. function:: group(*children, **name)

   Group several world objects together. All non keyword arguments somehow refer to
   world objects that will all be grouped together. An argument may be either a
   :class:`WorldObject`, the name of a world object or a sequence of world objects
   or names. The name of the new group  may be given via the ``name`` keyword
   argument. The return value is the newly created Group object.

   Example::

      >>> b1=Box()
      >>> b2=Box()
      >>> s1=Sphere()
      >>> listWorld()
      Root
      +---Box (Box/BoxGeom)
      +---Box1 (Box/BoxGeom)
      +---Sphere (Sphere/SphereGeom)
      >>> group("Box", "Box1", s1, name="MyGroup")
      >>> listWorld()
      Root
      +---MyGroup (Group/-)
          +---Box (Box/BoxGeom)
          +---Box1 (Box/BoxGeom)
          +---Sphere (Sphere/SphereGeom)

.. % ungroup()


.. function:: ungroup(group)

   Break up a group in its individual components. *group* is a group object or the
   name of a group object. This function does not only work with :class:`Group`
   objects but actually with any object that has no direct geometry assigned to it.

   Example::

      >>> listWorld()
      Root
      +---MyGroup (Group/-)
          +---Box (Box/BoxGeom)
          +---Box1 (Box/BoxGeom)
          +---Sphere (Sphere/SphereGeom)
      >>> ungroup("MyGroup")
      >>> listWorld()
      Root
      +---Box (Box/BoxGeom)
      +---Box1 (Box/BoxGeom)
      +---Sphere (Sphere/SphereGeom)

.. % replaceMaterial()


.. function:: replaceMaterial(name, newmat)

   Iterate over all world objects and replace each material called *name* with
   material object *newmat*.

.. % link()


.. function:: link(childs, parent=None, relative=False)

   Link the world objects *childs* to *parent*. Previously existing links are
   removed. If *parent* is ``None`` then the links are just removed. The argument
   *childs* may be either a single world object or a sequence of world objects.
   Instead of world objects you can also pass the names of the world objects.

   By default, the absolute position and orientation of the children is maintained
   (i.e. the local transform is modified). If you set *relative* to ``True`` the
   local transform is not modified which will change the position/orientation of
   the children (unless the parent transform is the identity).

   Note: The function modifies the name of a child object if there would be a clash
   with an existing object under the new parent.

.. % drawClear()


.. function:: drawClear()

   Clear all drawing objects.

.. % drawMarker()


.. function:: drawMarker(pos, col=(1,1,1), size=1)

   Draw a marker (a point). *col* is the color of the marker and *size* its radius.

.. % drawLine()


.. function:: drawLine(pos1, pos2, col=(1,1,1), size=1)

   Draw a line from *pos1* to *pos2*. *col* is the color of the  line and *size*
   its width.

.. % drawText()


.. function:: drawText(pos, txt, font=None, col=(1,1,1))

   Draw the text *txt* at position *pos* (3D position). *col* is the color of the
   text. The text is drawn using GLUT functionality. *font* is a GLUT font constant
   as defined in the :mod:`OpenGL.GLUT` module. It can take one of the following
   values:

* ``GLUT_BITMAP_8_BY_13``

* ``GLUT_BITMAP_9_BY_15`` (default)

* ``GLUT_BITMAP_TIMES_ROMAN_10``

* ``GLUT_BITMAP_TIMES_ROMAN_24``

* ``GLUT_BITMAP_HELVETICA_10``

* ``GLUT_BITMAP_HELVETICA_12``

* ``GLUT_BITMAP_HELVETICA_18``

.. % listWorld()


.. function:: listWorld()

   List the contents of the world as a tree. Example::

      >>> load("demo4.py")
      >>> listWorld()
      Root
      +---GLDistantLight (GLTargetDistantLight/-)
      +---GLDistantLight1 (GLTargetDistantLight/-)
      +---Sphere (Sphere/SphereGeom)
      |   +---Box0 (Box/BoxGeom)
      |   +---Box1 (Box/BoxGeom)
      |   +---Box2 (Box/BoxGeom)
      |   +---Box3 (Box/BoxGeom)
      |   +---Box4 (Box/BoxGeom)
      |   +---Box5 (Box/BoxGeom)
      |   +---Box6 (Box/BoxGeom)
      |   +---Box7 (Box/BoxGeom)
      |   +---Box8 (Box/BoxGeom)
      |   +---Box9 (Box/BoxGeom)
      +---TargetCamera (TargetCamera/-)

.. % load()


.. function:: load(filename, **options)

   Loads the given file without deleting the scene, so the contents of the file is
   appended to the current scene. Any additional keyword argument is considered to
   be an option and is passed to the importer.

   To be able to load the file there must be an appropriate import class (protocol:
   "Import") available in the plugin manager. The class is determined by examining
   the file extension. If no importer is found a :exc:`NoImporter` exception is
   thrown. See chapter :ref:`importplugins` for the available standard importers.

   Any exception generated in the importer is passed to the caller.

.. % save()


.. function:: save(filename, **options)

   Saves the current scene. Any additional keyword argument is considered to be an
   option and is passed to the exporter.

   To be able to save the scene there must be an appropriate export class
   (protocol: "Export") available in the plugin manager. The class is determined by
   examining the file extension. If no exporter is found a :exc:`NoExporter`
   exception is thrown.

   Any exception generated in the exporter is passed to the caller.

.. % reset()


.. function:: reset()

   Reset an animation/simulation. This function sets the global time back to 0 and
   signals the RESET event.

.. % worldObject()


.. function:: worldObject(obj)

   If *obj* is a string type this function searches the world object with that name
   and returns it. If *obj* is not a string, it is returned unchanged.

.. % worldObjects()


.. function:: worldObjects(objs)

   Similar to :func:`worldObject` but the argument may be a sequence of objects or
   strings. The return value is always a list, even if only one object was
   specified as input.

.. % register()


.. function:: register(cmds,...)

   Register functions in the cmds module. The function takes an arbitrary number of
   arguments which must be callable objects. Each such function is registered in
   the cmds module, so that other modules can call these functions as if they were
   implemented directly in the cmds module.

.. % importDefaultPlugins


.. function:: importDefaultPlugins(paths)

   Import the default plugins. The plugin files/directories specified by the
   :envvar:`CGKIT_PLUGIN_PATH` environment variable (if it exists) are imported.
   The function already outputs error messages and returns a list of plugin
   descriptors.

.. % splitPaths


.. function:: splitPaths(paths)

   Split a string containing paths into the individual paths. The paths can either
   be separated by ``':'`` or ``';'``. Windows drive letters are maintained, even
   when ``':'`` is used as separator. ::

      >>> splitPaths("&:c:\\shaders:c:\\more_shaders;")
      ['&', 'c:\\shaders', 'c:\\more_shaders']    

