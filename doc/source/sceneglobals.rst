:mod:`sceneglobals` --- Setting global scene data
=================================================

.. module:: cgkit.sceneglobals
   :synopsis: Setting global scene data

The :class:`Globals` class is a convenience class that just sets the global
attributes in the scene object. You can use it as a shortcut to set the
attributes.


.. class:: Globals(up = (0,0,1), handedness = 'r', unit = 'm', unitscale = 1.0, **keyargs)

   *up* is the global "up" direction.

   *handedness* specifies whether a left handed or right handed coordinate system
   should be used.

   *unit* and *unitscale* specify what 1 unit in 3D space would  be in the real
   world.

   Any additional argument is stored in the scene as a global option and can be
   retrieved using the :meth:`getGlobal()<cgkit.scene.Scene.getGlobal>` method of the scene object.

