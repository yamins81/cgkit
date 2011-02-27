.. % TriMeshGeom


:class:`TriMeshGeom` --- Triangle mesh geometry
===============================================


.. class:: TriMeshGeom()

   Creates an empty triangle mesh.

A :class:`TriMeshGeom` has the following slots:

+------------------------+--------------+--------+--------------------------+
| Slot                   | Type         | Access | Description              |
+========================+==============+========+==========================+
| ``cog_slot``           | vec3         | r      | The local center of      |
|                        |              |        | gravity                  |
+------------------------+--------------+--------+--------------------------+
| ``inertiatensor_slot`` | mat3         | r      | The local inertia tensor |
+------------------------+--------------+--------+--------------------------+
| ``verts_slot``         | vec3 array   | rw     | The mesh vertices        |
+------------------------+--------------+--------+--------------------------+
| ``faces_slot``         | int[3] array | rw     | The mesh faces           |
+------------------------+--------------+--------+--------------------------+

.. % Attributes


.. attribute:: TriMeshGeom.cog

   Center of gravity with respect to the local coordinate system of the  triangle
   mesh.


.. attribute:: TriMeshGeom.inertiatensor

   Inertia tensor with respect to the local coordinate system of the  triangle
   mesh.


.. attribute:: TriMeshGeom.verts

   This attribute contains the sequence of mesh vertices.


.. attribute:: TriMeshGeom.faces

   This attribute contains the sequence of mesh faces. Each face contains three
   vertex indices.

.. % Methods


.. method:: TriMeshGeom.intersectRay(origin, direction, earlyexit=False)

   Intersect a ray with the mesh. The ray starts at *origin* and travels along
   *direction* which must both be of type :class:`vec3`. *origin* and *direction*
   must be given with respect to the local coordinate system L of the geometry. If
   *earlyexit* is ``True`` the method returns after the first hit, otherwise all
   triangles are tested and the result contains the nearest hit. The return value
   is a tuple (*hit*, *t*, *faceindex*,  *u*, *v*) where *hit* is a boolean that
   indicates if the mesh was hit or not. *t* is the ray parameter, i.e. the point
   of intersection is at *origin* + t\**direction*. *faceindex* is the index of the
   face that was hit and *u* *v* are the parameter coordinates of the intersection
   point.

   This method tests the ray with all triangles, so it is not efficient if you have
   a lot of rays to test. It is meant for only a few rays where the preprocessing
   cost wouldn't be amortized.

   The ray-triangle intersection code (non-culling case) is based on:

     | Tomas Möller and Ben Trumbore 
     | *Fast, minimum storage ray-triangle intersection*
     | Journal of graphics tools, 2(1):21-28, 1997 ---
     | `<http://jgt.akpeters.com/papers/MollerTrumbore97/>`_

