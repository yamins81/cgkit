.. % Joint


:class:`Joint` --- Joint class for creating a skeleton
======================================================

The :class:`Joint` class can be used to create articulated characters. Bones are
implicitely created by linking two joints, i.e. a bone is assumed between a
joint and each of its children joints. A :class:`Joint` object corresponds to a
ball joint that has three rotational degrees of freedom. The actual joint is
always located at the pivot point of the :class:`Joint` object and rotates about
the pivot frame axes.

If you want your local coordinate frame to be oriented differently, it is not
enough to set the offset transform as this will also readjust the local frame so
that the effect is actually cancelled out and you will still rotate about the
same axes as before. To initialize the new frame you have to call
:meth:`freezePivot` after setting the offset transform. This will set the
current offset transform as the new default pose where all angles are 0 and will
make the joint rotate about the new axes. After freezing you may want to set the
offset transform back to the identity so that the local coordinate frame and the
offset frame coincide again. This won't affect the rotation of the joint but the
location of its children (as setting the offset transform on a *Joint* object
also modifies its local coordinate system). But it actually depends on the
situation whether you have to reset the offset transform or not.

Rotating a joint is not done by setting its *rot* attribute but by setting its
three individual angles (*anglex*, *angley*,  *anglez*).


.. class:: Joint(name = "",  radius = 0.05,  rotationorder = "xyz" )

   *name* is the name of the joint.

   *radius* is the radius of the visual representation of the joint/bone.

   *rotationorder* determines the order of rotation about the individual axes.

A :class:`Joint` has the following slots:

+-----------------+-------+--------+---------------------+
| Slot            | Type  | Access | Description         |
+=================+=======+========+=====================+
| ``anglex_slot`` | float | rw     | Angle around x axis |
+-----------------+-------+--------+---------------------+
| ``angley_slot`` | float | rw     | Angle around y axis |
+-----------------+-------+--------+---------------------+
| ``anglez_slot`` | float | rw     | Angle around z axis |
+-----------------+-------+--------+---------------------+


.. attribute:: Joint.anglex

   Rotation angle (in degrees) about the local x axis.


.. attribute:: Joint.angley

   Rotation angle (in degrees) about the local y axis.


.. attribute:: Joint.anglez

   Rotation angle (in degrees) about the local z axis.

.. % Methods


.. method:: Joint.freezePivot()

   Make the current pivot coordinate system the default pose. After calling this
   method, the current rotation of the pivot coordinate system will define the
   default pose. This means, rotations are now defined around the local pivot axes.

