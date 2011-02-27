.. % WorldObject


:class:`WorldObject` --- World object base class
================================================

A *world object* is anything that is part of the 3D scene, i.e. it has a
coordinate system associated with it (geometry, camera, lights). The
:class:`WorldObject` class is derived from the :class:`Component` class.

.. class:: WorldObject(name = "object", transform = None, pos = None,  rot = None, scale = None, pivot = None, offsetTransform = None, parent = None, mass = None, material = None, visible = True, auto_insert = True)

   *name* is the name of the object which can be used to identify the object.

   *transform* is the initial transformation that should be applied to the object.
   Alternatively, you can specify the individual components *pos*, *rot* and
   *scale*.

   *pivot* is the pivot point of the object. This is the 4th column of the offset
   transformation. You can also specify the entire offset  transformation using the
   *offsetTransform* argument.

   *parent* is the parent world object and determines at which position in the
   scene graph the new object is added. If *parent* is ``None`` the object will
   become a child of the world root. The *parent* argument is only used if
   *auto_insert* is ``True``.

   *mass* is the mass of this object (this does not include the children objects).

   *material* describes the appearance of the object. It can be either  a single
   :class:`Material` object or a sequence of :class:`Material` objects.

   *visible* is a flag that determines whether the object is visible or not. This
   only affects the geometry of this WorldObject, it is not inherited by children
   objects.

   The object will be inserted into the scene automatically if  *auto_insert* is
   set to ``True``.

A :class:`WorldObject` always has the following slots:

+-------------------------+-------+--------+---------------------------+
| Slot                    | Type  | Access | Description               |
+=========================+=======+========+===========================+
| ``angularvel_slot``     | vec3  | rw     | Angular velocity          |
+-------------------------+-------+--------+---------------------------+
| ``cog_slot``            | vec3  | r      | Center of gravity         |
+-------------------------+-------+--------+---------------------------+
| ``inertiatensor_slot``  | mat3  | r      | Inertia tensor            |
+-------------------------+-------+--------+---------------------------+
| ``linearvel_slot``      | vec3  | rw     | Linear velocity           |
+-------------------------+-------+--------+---------------------------+
| ``mass_slot``           | float | rw     | Mass of the local         |
|                         |       |        | geometry                  |
+-------------------------+-------+--------+---------------------------+
| ``pos_slot``            | vec3  | rw     | Position                  |
+-------------------------+-------+--------+---------------------------+
| ``rot_slot``            | mat3  | rw     | Orientation               |
+-------------------------+-------+--------+---------------------------+
| ``scale_slot``          | vec3  | rw     | Scaling                   |
+-------------------------+-------+--------+---------------------------+
| ``totalmass_slot``      | float | r      | Total mass (including the |
|                         |       |        | children)                 |
+-------------------------+-------+--------+---------------------------+
| ``transform_slot``      | mat4  | rw     | Object transformation     |
+-------------------------+-------+--------+---------------------------+
| ``visible_slot``        | bool  | rw     | Visibility flag           |
+-------------------------+-------+--------+---------------------------+
| ``worldtransform_slot`` | mat4  | rw     | World transformation      |
+-------------------------+-------+--------+---------------------------+


.. attribute:: WorldObject.geom

   This attribute holds the visible geometry which must be derived from
   :class:`GeomObject`. The value can also be ``None`` if there is no visible
   geometry. Geometry objects can be shared between different  world objects. This
   value can be read and written.


.. attribute:: WorldObject.parent

   This attribute contains the parent world object or ``None``. You can  only read
   this attribute.


.. attribute:: WorldObject.transform

   This is the value of the mat4 slot *transform_slot* which contains the object
   transformation T. You can read and write this attribute.


.. attribute:: WorldObject.worldtransform

   This is the value of the mat4 slot *worldtransform_slot* which contains the
   world transformation (which is a concatenation of all local transformations L).
   You can only read this attribute.

   .. note::

      Note that in contrast to the *transform* slot, the *worldtransform* is not
      influenced by the offset transformation.


.. attribute:: WorldObject.pos

   This is the value of the vec3 slot *pos_slot* which contains the  position of
   the object. You can read and write this attribute.


.. attribute:: WorldObject.rot

   This is the value of the mat3 slot *rot_slot* which contains the  orientation of
   the object. You can read and write this attribute.


.. attribute:: WorldObject.scale

   This is the value of the vec3 slot *scale_slot* which contains the  scaling of
   the object. You can read and write this attribute.


.. attribute:: WorldObject.pivot

   This is the pivot point (vec3) of the object. You can read and write this
   attribute. Reading or writing this attribute is equivalent to calling
   :meth:`getOffsetTransform` or :meth:`setOffsetTransform` with a matrix that only
   modifies the 4th column.


.. attribute:: WorldObject.cog

   This is the value of the vec3 slot *cog_slot* which contains the physical center
   of gravity. This value is derived from the center of  gravity provided by the
   geometry object and the cogs and masses of the children objects. This means it
   represents the center of gravity of the  entire hierarchy. The value is given
   with respect to the pivot coordinate system P. You can only read this value.


.. attribute:: WorldObject.inertiatensor

   This is the value of the mat3 slot *inertiatensor_slot* which contains the
   inertia tensor of the entire hierarchy (just like *cog*). You can only read this
   value.


.. attribute:: WorldObject.mass

   This is the value of the double slot *mass_slot* which contains the local mass
   of this object (not including the children). Or in other words, this is the mass
   of the geometry directly set in this object. You can read and write this value.


.. attribute:: WorldObject.totalmass

   This is the value of the double slot *totalmass_slot* which contains the total
   mass of this object and its children. You can only read this value.


.. attribute:: WorldObject.angularvel

   This is the value of the vec3 slot *angular_slot* which contains the  angular
   velocity of the object. The value is not computed but has to be set by anyone
   who knows the angular velocity (such as a dynamics component). You can read and
   write this attribute.


.. attribute:: WorldObject.linearvel

   This is the value of the vec3 slot *linearvel_slot* which contains the  linear
   velocity of the object. The value is not computed but has to be set by anyone
   who knows the linear velocity (such as a dynamics component). You can read and
   write this attribute.

.. % Methods


.. method:: WorldObject.boundingBox()

   Return the local axis aligned bounding box. The bounding box is given with
   respect to the local transformation L (which is not what you get from the
   transform slot of the world object).


.. method:: WorldObject.localTransform()

   Returns the local transformation that has to be used for rendering. The returned
   transformation L is calculated as follows: :math:`L = T\cdot P^{-1}`, where T is
   the current transform (taken from the transform slot) and P is the offset
   transform.


.. method:: WorldObject.getOffsetTransform()

   Return the current offset transformation as a :class:`mat4`. This transformation
   is given relative to the local object transformation.


.. method:: WorldObject.setOffsetTransform(P)

   Set the offset transformation. The transformation has to be given relative to
   the local object transformation. After setting the offset transformation, the
   transform slot will be updated so that  :meth:`localTransform` returns the same
   matrix as before, i.e. the world position/orientation of the object does not
   change.


.. method:: WorldObject.getNumMaterials()

   Return the current size of the material array.


.. method:: WorldObject.setNumMaterials(num)

   Set a new size for the material array.


.. method:: WorldObject.getMaterial(idx=0)

   Get a stored material. The method returns ``None`` if the given index is out of
   range or there is no material stored at that position.


.. method:: WorldObject.setMaterial(mat, idx=0)

   Set a new material. An :exc:`IndexError` exception is thrown if the index is out
   of range.


.. method:: WorldObject.lenChilds()

   Return the number of children objects.


.. method:: WorldObject.iterChilds()

   Return an iterator that iterates over all children objects.


.. method:: WorldObject.hasChild(name)

   Check if a children with a particular name does exist.


.. method:: WorldObject.child(name)

   Return the children with a particluar name. A :exc:`KeyError` exception is
   thrown if there is no children with the specified name.


.. method:: WorldObject.addChild(child)

   Add a new children world object to this object. A :exc:`ValueError` exception is
   thrown if child was already added to another object.  In this case you have to
   remove the object from its previous parent yourself. You also have to make sure
   that the name of *child* is unique among the children of this object, otherwise
   a :exc:`KeyError` exception is thrown.


.. method:: WorldObject.removeChild(child)

   Remove a children world object from this object. *child* can either be the name
   of the children or the object itself. A :exc:`KeyError` exception is thrown if
   child is not a children of this object.


.. method:: WorldObject.makeChildNameUnique(name)

   Modify *name* so that it is unique among the children names. If *name* is
   already the name of a children object, then it is modified by adding/increasing
   a trailing number, otherwise it is returned unchanged.

