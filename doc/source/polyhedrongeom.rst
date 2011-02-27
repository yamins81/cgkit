.. % PolyhedronGeom


:class:`PolyhedronGeom` --- Polyhedron geometry
===============================================

The :class:`PolyhedronGeom` class stores a collection of general planar concave
polygons that may also contain holes. Each polygon is described by a sequence of
vertex loops. The first loop defines the polygon boundary and each subsequent
loop describes a hole. Each loop is a sequence of vertex indices.


.. class:: PolyhedronGeom()

   Creates an empty polyhedron.

A :class:`PolyhedronGeom` has the following slots:

+----------------+------------+--------+----------------------+
| Slot           | Type       | Access | Description          |
+================+============+========+======================+
| ``verts_slot`` | vec3 array | rw     | The polygon vertices |
+----------------+------------+--------+----------------------+

.. % Attributes


.. attribute:: PolyhedronGeom.verts

   This attribute contains the sequence of polygon vertices.

.. % Methods


.. method:: PolyhedronGeom.hasPolysWithHoles()

   Return ``True`` if there is at least one polygon with a hole.


.. method:: PolyhedronGeom.getNumPolys()

   Return the number of polygons.


.. method:: PolyhedronGeom.getNumLoops(poly)

   Return the number of vertex loops in the polygon with index *poly*.


.. method:: PolyhedronGeom.getNumVerts(poly, loop)

   Return the number of vertex indices in one particular loop. *poly* is the
   polygon index and *loop* the loop index.


.. method:: PolyhedronGeom.setNumPolys(num)

   Allocate space for *num* polygons.


.. method:: PolyhedronGeom.setNumLoops(poly, num)

   Allocate space for *num* loops in the polygon with index *poly*.


.. method:: PolyhedronGeom.getLoop(poly, loop)

   Return a loop from a polygon. *poly* is the polygon index and *loop* the loop
   index. The return value is a sequence of vertex indices.


.. method:: PolyhedronGeom.setLoop(poly, loop, vloop)

   Set a new polygon loop. *poly* is the polygon index, *loop* the loop index and
   *vloop* a sequence of vertex indices.


.. method:: PolyhedronGeom.getPoly(poly)

   Return a polygon. *poly* is the polygon index. The return value is a sequence of
   vertex loops.


.. method:: PolyhedronGeom.setPoly(poly, polydef)

   Set a polygon. *poly* is the polygon index and *polydef* a sequence of vertex
   loops.

