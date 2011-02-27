.. % BoxGeom


:class:`BoxGeom` --- Box geometry
=================================


.. class:: BoxGeom(lx=1.0, ly=1.0, lz=1.0, segmentsx=1, segmentsy=1, segmentsz=1)

   *lx*, *ly* and *lz* specify the dimensions of the box in the respective
   direction.

   *segmentsx*, *segmentsy* and *segmentsz* are the number of segments in each
   direction.

A :class:`BoxGeom` has the following slots:

+------------------------+-------+--------+---------------------------+
| Slot                   | Type  | Access | Description               |
+========================+=======+========+===========================+
| ``cog_slot``           | vec3  | r      | The local center of       |
|                        |       |        | gravity                   |
+------------------------+-------+--------+---------------------------+
| ``inertiatensor_slot`` | mat3  | r      | The local inertia tensor  |
+------------------------+-------+--------+---------------------------+
| ``lx_slot``            | float | rw     | The length in x direction |
+------------------------+-------+--------+---------------------------+
| ``ly_slot``            | float | rw     | The length in y direction |
+------------------------+-------+--------+---------------------------+
| ``lz_slot``            | float | rw     | The length in z direction |
+------------------------+-------+--------+---------------------------+
| ``segmentsx_slot``     | int   | rw     | The number of segments in |
|                        |       |        | x direction               |
+------------------------+-------+--------+---------------------------+
| ``segmentsy_slot``     | int   | rw     | The number of segments in |
|                        |       |        | y direction               |
+------------------------+-------+--------+---------------------------+
| ``segmentsz_slot``     | int   | rw     | The number of segments in |
|                        |       |        | z direction               |
+------------------------+-------+--------+---------------------------+

.. % Attributes


.. attribute:: BoxGeom.cog

   Center of gravity with respect to the local coordinate system.


.. attribute:: BoxGeom.inertiatensor

   Inertia tensor with respect to the local coordinate system.


.. attribute:: BoxGeom.lx

   The length in x direction.


.. attribute:: BoxGeom.ly

   The length in y direction.


.. attribute:: BoxGeom.lz

   The length in z direction.


.. attribute:: BoxGeom.segmentsx

   The number of segments in x direction.


.. attribute:: BoxGeom.segmentsy

   The number of segments in y direction.


.. attribute:: BoxGeom.segmentsz

   The number of segments in z direction.

