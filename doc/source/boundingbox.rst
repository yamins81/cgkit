.. % BoundingBox

:mod:`boundingbox` --- An axis-aligned bounding box
===================================================

.. module:: cgkit.boundingbox
   :synopsis: An axis-aligned bounding box


.. class:: BoundingBox([b1, b2])

   *b1* and *b2* are the initial bounds (as :class:`vec3`) of  the bounding box.
   Everything that lies within the specified box (including the bounds) is
   considered inside the bounding box. If you don't specify any bounds the bounding
   box is initially empty.


.. method:: BoundingBox.clear()

   Make the bounding box empty.


.. method:: BoundingBox.isEmpty()

   Return ``True`` if the bounding box is empty.


.. method:: BoundingBox.getBounds([dir])

   Return the minimum and maximum bound. The bounds are returned as :class:`vec3`
   objects. The optional vec3 argument *dir* specifies which corners are actually
   returned. When viewed along a ray with the given direction the minimum bound
   always comes before the maximum bound.


.. method:: BoundingBox.setBounds(b1, b2)

   Set new bounds for the bounding box. The rectangle given by *b1* and *b2*
   defines the new bounding box.


.. method:: BoundingBox.center()

   Return the center of the bounding box (i.e. 0.5\*(bmin+bmax)). The null vector
   is returned if the bounding box is empty.


.. method:: BoundingBox.addPoint(p)

   Enlarge the bounding box so that the point *p* is enclosed in the box.


.. method:: BoundingBox.addBoundingBox(bb)

   Enlarge the bounding box so that the bounding box *bb* is enclosed in the box.


.. method:: BoundingBox.transform(M)

   Returns a transformed bounding box. The transformation is given by *M* which
   must be a :class:`mat4`. The result will still be axis aligned, so the volume
   will not be preserved.


.. method:: BoundingBox.clamp(p)

   Clamp the point *p* so that it lies within the bounding box. Each axis is
   clamped against the corresponding bounding box interval.

