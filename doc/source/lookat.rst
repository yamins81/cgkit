.. % LookAt component


:class:`LookAt` --- Align the orientation of objects so they point to a particular target
=========================================================================================

The :class:`LookAt` component takes a position and target point and  calculates
a rotation matrix that orients an object positioned at the input position so
that the z axis points to target.

The input slots are *pos_slot* and *target_slot* and the output slot is
*output_slot*.


.. class:: LookAt(name = "LookAt",  pos = (0,0,0),  target = (0,0,0),  up = (0,0,1),  roll = 0.0,  auto_insert = True)

   *pos* is the position of the object.

   *target* is the target point that should be aimed at.

   *up* is the vector that is considered to be the 'up' direction.

   *roll* is an angle in degrees that the object is rotated around its local z
   axis.

