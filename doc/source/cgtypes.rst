
:mod:`cgtypes` --- Vectors, matrices and quaternions
====================================================

.. module:: cgkit.cgtypes
   :synopsis: Basic types useful for computer graphics.

.. toctree::

   vec3
   vec4
   mat3
   mat4
   quat


This module contains 3D/4D vector, matrix and quaternion types:

* vec3 -- a three dimensional vector type to store points, vectors, normals or
  even colors.

* vec4 -- a four dimensional vector type to store homogenous points, for
  example.

* mat3 -- a 3x3 matrix to store linear transformations.

* mat4 -- a 4x4 matrix to store affine transformations.

* quat -- a quaternion type as a specialized way to store rotations.

You import all of those types at once with ::

   from cgkit.cgtypes import *

or you can import them individually like this ::

   from cgkit.cgtypes import vec3, mat4

In general, you can use those types just as if they were built-in types which
means the mathematical operators can be used and have their respective meaning.
Each type has some additional methods which are described in the respective
documentation.

Here are some examples::

   >>> from cgkit.cgtypes import *
   >>> v=vec3(0.5,1.0,-2.5)
   >>> print v
   (0.5000, 1.0000, -2.5000)
   >>> print v.length()
   2.73861278753
   >>> v=v.normalize()
   >>> print v
   (0.1826, 0.3651, -0.9129)
   >>> print v.length()
   1.0

Now let's construct a rotation matrix that rotates points by 90 degrees around
the z-axis::

   >>> M=mat4(1).rotate(0.5*math.pi, vec3(0,0,1))

and apply the rotation to the vector (1,0,0) (the x-axis)::

   >>> print M*vec3(1,0,0)
   (0.0000, 1.0000, 0.0000)

The module contains the following functions:


.. function:: getEpsilon()

   Return the epsilon threshold which is used for doing comparisons.


.. function:: setEpsilon(eps)

   Sets a new epsilon threshold and returns the previously set value. Two values
   are considered to be equal if their absolute difference is less than or equal to
   epsilon.


.. function:: slerp(t, q0, q1, shortest=True)

   Performs a spherical linear interpolation between two quaternions *q0* and *q1*.
   For *t*=0.0 the return value equals *q0*, for  *t*=1.0 it equals *q1*. *q0* and
   *q1* must be unit  quaternions. If *shortest* is ``True`` the interpolation will
   always be along the shortest path, otherwise it depends on the orientation of
   the input quaternions whether the shortest or longest path will be taken (you
   can switch between the paths by negating either *q0* or *q1*).


.. function:: squad(t, a, b, c, d)

   Performs a spherical cubic interpolation between quaternion *a* and *d* where
   quaternion *b* and *c* define the shape of the interpolation curve. For *t*=0.0
   the return value equals *a*, for *t*=1.0 it  equals *d*. All quaternions must be
   unit quaternions.


