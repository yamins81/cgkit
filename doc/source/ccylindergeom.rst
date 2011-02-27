.. % CCylinderGeom


:class:`CCylinderGeom` --- Capped cylinder geometry
===================================================


.. class:: CCylinderGeom(radius=1.0, length=1.0, segmentsu=16, segmentsvl=1, segmentsvr=4)

   *radius* is the radius of the cylinder and caps in the local coordinate  system.

   *length* is the length of the cylinder (not counting the caps).

   *segmentsu* is the number of segments in u direction, *segmentsvl* the number of
   segments of the cylinder part and *segmentsvr* the number of segments for each
   cap.

A :class:`CCylinderGeom` has the following slots:

+------------------------+-------+--------+---------------------------+
| Slot                   | Type  | Access | Description               |
+========================+=======+========+===========================+
| ``cog_slot``           | vec3  | r      | The local center of       |
|                        |       |        | gravity                   |
+------------------------+-------+--------+---------------------------+
| ``inertiatensor_slot`` | mat3  | r      | The local inertia tensor  |
+------------------------+-------+--------+---------------------------+
| ``radius_slot``        | float | rw     | The radius of the         |
|                        |       |        | cylinder                  |
+------------------------+-------+--------+---------------------------+
| ``length_slot``        | float | rw     | The lenght of the         |
|                        |       |        | cylinder                  |
+------------------------+-------+--------+---------------------------+
| ``segmentsu_slot``     | int   | rw     | The number of segments in |
|                        |       |        | u direction               |
+------------------------+-------+--------+---------------------------+
| ``segmentsvl_slot``    | int   | rw     | The number of cylinder    |
|                        |       |        | segments in v direction   |
+------------------------+-------+--------+---------------------------+
| ``segmentsvr_slot``    | int   | rw     | The number of cap         |
|                        |       |        | segments in v direction   |
+------------------------+-------+--------+---------------------------+

.. % Attributes


.. attribute:: CCylinderGeom.cog

   Center of gravity with respect to the local coordinate system.


.. attribute:: CCylinderGeom.inertiatensor

   Inertia tensor with respect to the local coordinate system.


.. attribute:: CCylinderGeom.radius

   The radius of the cylinder and caps.


.. attribute:: CCylinderGeom.length

   The length of the cylinder (not counting the caps).


.. attribute:: CCylinderGeom.segmentsu

   The number of segments in u direction.


.. attribute:: CCylinderGeom.segmentsvl

   The number of cylinder segments in v direction.


.. attribute:: CCylinderGeom.segmentsvr

   The number of cap segments in v direction.

