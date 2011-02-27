.. % SphereGeom


:class:`SphereGeom` --- Sphere geometry
=======================================


.. class:: SphereGeom(radius=1.0, segmentsu=16, segmentsv=8)

   *radius* is the radius in the local coordinate system.

   *segmentsu* and *segmentsv* are used when the sphere has to be tesselated
   (either for interactive display or when converted to a triangle mesh).

A :class:`SphereGeom` has the following slots:

+------------------------+-------+--------+---------------------------+
| Slot                   | Type  | Access | Description               |
+========================+=======+========+===========================+
| ``cog_slot``           | vec3  | r      | The local center of       |
|                        |       |        | gravity                   |
+------------------------+-------+--------+---------------------------+
| ``inertiatensor_slot`` | mat3  | r      | The local inertia tensor  |
+------------------------+-------+--------+---------------------------+
| ``radius_slot``        | float | rw     | The radius                |
+------------------------+-------+--------+---------------------------+
| ``segmentsu_slot``     | int   | rw     | The number of segments in |
|                        |       |        | u direction               |
+------------------------+-------+--------+---------------------------+
| ``segmentsv_slot``     | int   | rw     | The number of segments in |
|                        |       |        | v direction               |
+------------------------+-------+--------+---------------------------+

.. % Attributes


.. attribute:: SphereGeom.cog

   Center of gravity with respect to the local coordinate system.


.. attribute:: SphereGeom.inertiatensor

   Inertia tensor with respect to the local coordinate system.


.. attribute:: SphereGeom.radius

   The radius of the sphere.


.. attribute:: SphereGeom.segmentsu

   The number of segments in u direction.


.. attribute:: SphereGeom.segmentsv

   The number of segments in v direction.

