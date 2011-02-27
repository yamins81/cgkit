:mod:`scene` --- Storing 3D data
================================

.. module:: cgkit.scene
   :synopsis: Storing 3D data

The :class:`Scene` class is a container for anything that should also be visible
to other objects. The "visible" part of the scene is called the world which is
what gets rendered on screen (everything that has a coordinate system is part of
the world). However, the scene can also contain objects that are not part of the
world. For example, the global timer or any other object that is used somehow
but has no direct visible representation.

You can retrieve the global scene object by calling the function
:func:`getScene`.


.. function:: getScene()

   Returns the global scene object.

If a script is invoked via a tool such as the viewer or render tool, the
variable ``scene`` is automatically initialized with the global scene object. To
inspect what objects the scene currently contains, you can just iterate over a
scene::

   >>> for item in scene: print item
   ...
   <cgkit.timer.Timer object at 0x0907FEA0>
   <cgkit.worldobject.WorldObject object at 0x0907FE70>

This is what an empty scene looks like. The two scene items you see here are the
globel timer and the root object of the world. These objects are automatically
created by the scene.


.. class:: Scene()

.. attribute:: Scene.handedness

   Specifies if the scene uses a left-handed or right-handed coordinate system. The
   value is a string that is either be ``'l'`` or ``'r'``. The default is right
   handed.


.. attribute:: Scene.up

   Specifies the direction that is considered to be the "up" direction. The  value
   must be a :class:`vec3`. The default is (0,0,1) (z axis).


.. attribute:: Scene.unit


.. attribute:: Scene.unit_scale


.. method:: Scene.worldRoot()

   Return the root world object.


.. method:: Scene.walkWorld(root=None)

   Walk the world tree and yield each object. This method can be used to iterator
   over the entire world tree or a subtree thereof. The argument  *root* specifies
   the root of the tree which is to traverse (the  root itself will not be
   returned).


.. method:: Scene.timer()

   Return the global :class:`Timer<cgkit.timer.Timer>` object.


.. method:: Scene.clear()

   Clear the entire scene.


.. method:: Scene.insert(item)

   Insert an item into the scene.


.. method:: Scene.item(name)

   Return the item with the specified name.


.. method:: Scene.worldObject(name)

   Return the world object with the specified name. You can use the character
   ``'|'`` as a path separator.


.. method:: Scene.boundingBox()

   Return the bounding box of the entire scene.


.. method:: Scene.setJoystick(joystick)

   Set a joystick object.


.. method:: Scene.getJoystick(id)

   Get a :class:`Joystick<cgkit.joystick.Joystick>` object. A dummy joystick object is
   returned if there is no joystick with the specified id.


.. method:: Scene.hasGlobal(name)

   Return ``True`` if a global option with name *name* exists.


.. method:: Scene.getGlobal(name, default=None)

   Get the global option with the given name. *default* is returned if the option
   does not exist. The options can be set using the  :class:`Globals` class or the
   :meth:`setGlobal` method.


.. method:: Scene.setGlobal(name, value)

   Set the global option *name* to *value*.

