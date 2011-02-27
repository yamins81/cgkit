.. % PlaneGeom


:class:`PlaneGeom` --- Plane geometry
=====================================


.. class:: PlaneGeom(lx=1.0, ly=1.0, segmentsx=1, segmentsy=1)

   *lx* and *ly* specify the dimensions of the plane in the respective direction.

   *segmentsx* and *segmentsy* are the number of segments in each direction.

A :class:`PlaneGeom` has the following slots:

+--------------------+-------+--------+---------------------------+
| Slot               | Type  | Access | Description               |
+====================+=======+========+===========================+
| ``lx_slot``        | float | rw     | The length in x direction |
+--------------------+-------+--------+---------------------------+
| ``ly_slot``        | float | rw     | The length in y direction |
+--------------------+-------+--------+---------------------------+
| ``segmentsx_slot`` | int   | rw     | The number of segments in |
|                    |       |        | x direction               |
+--------------------+-------+--------+---------------------------+
| ``segmentsy_slot`` | int   | rw     | The number of segments in |
|                    |       |        | y direction               |
+--------------------+-------+--------+---------------------------+

.. % Attributes


.. attribute:: PlaneGeom.lx

   The length in x direction.


.. attribute:: PlaneGeom.ly

   The length in y direction.


.. attribute:: PlaneGeom.segmentsx

   The number of segments in x direction.


.. attribute:: PlaneGeom.segmentsy

   The number of segments in y direction.

