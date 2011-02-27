
.. _quat:

quat - quaternions
------------------

A :class:`quat` represents a quaternion type that can be used to store
rotations. A quaternion contains four values of which one can be seen as the
angle and the other three as the axis of rotation. The most common way to
initialize a quaternion is by specifying an angle (in radians) and the axis of
rotation::

   # initialize the quaternion by specifying an angle and the axis of rotation
   q = quat(0.5*pi, vec3(0,0,1))

   # initialize by specifying a rotation matrix (as mat3 or mat4)
   q = quat(R)

   # all components are set to zero
   q = quat()

   (0.0000, 0.0000, 0.0000, 0.0000)

   # set the w component
   q = quat(2.5)

   (0.5000, 0.0000, 0.0000, 0.0000)

   # set all four components (w,x,y,z)
   q = quat(1,0,0,0)
   q = quat([1,0,0,0])
   q = quat("1,0,0,0")

   (1.0000, 0.0000, 0.0000, 0.0000)

Finally, you can initialize a quaternion with a copy of another quaternion::

   q = quat(r)

**Mathematical operations:**

The mathematical operators are supported with the following combination of
types::

   quat  =  quat + quat
   quat  =  quat - quat
   quat  =  quat * quat
   quat  = float * quat
   quat  =  quat * float
   quat  =  quat / float
   quat  = -quat
   quat  =  quat ** float = pow(quat, float)   (new in version 1.1)
   quat  =  quat ** quat  = pow(quat, quat)    (new in version 1.1)

Additionally, you can compare quaternions with ``==`` and ``!=``.  Taking the
absolute value will return the magnitude of the quaternion::

   float = abs(q)


.. class:: quat([args])


   .. method:: quat.conjugate()

      Return the conjugate :math:`(w, -x, -y, -z)` of the quaternion.


   .. method:: quat.normalize()

      Returns the normalized quaternion. If the method is called on the null vector a
      :exc:`ZeroDivisionError` is raised.


   .. method:: quat.inverse()

      Return the inverse of the quaternion.


   .. method:: quat.toAngleAxis()

      Returns a tuple containing the angle (in radians) and the axis of rotation. The
      returned axis can also be zero if the rotation is actually the identity.


   .. method:: quat.fromAngleAxis(angle, axis)

      Initializes *self* from an angle (in radians) and an axis of rotation and
      returns *self*. The initialized quaternion will be a unit quaternion. Passing
      the null vector as axis has the same effect as passing an angle of 0 (i.e. the
      quaternion will be set to (1,0,0,0)).


   .. method:: quat.toMat3()

      Convert the quaternion into a rotation matrix and return the matrix as a
      :class:`mat3`.


   .. method:: quat.toMat4()

      Convert the quaternion into a rotation matrix and return the matrix as a
      :class:`mat4`.


   .. method:: quat.fromMat(matrix)

      Initialize *self* from a rotation matrix, given either as a :class:`mat3` or a
      :class:`mat4` and returns *self*. *matrix* must be a rotation matrix (i.e. the
      determinant is 1), if you have a matrix that is made up of other parts as well,
      call :meth:`matrix.decompose` to get the rotation part.


   .. method:: quat.dot(b)

      Returns the dot product of *self* with quaternion *b*. ---  New in version 1.1.


   .. method:: quat.log()

      Returns the natural logarithm of *self*. ---  New in version 1.1.


   .. method:: quat.exp()

      Returns the exponential of *self*.  ---  New in version 1.1.


   .. method:: quat.rotateVec(v)

      Return the rotated vector *v*. The quaternion must be a unit quaternion. This
      operation is equivalent to turning *v* into a quat, computing
      ``self*v*self.conjugate()`` and turning the result back into a vec3. ---  New in
      version 2.0.

**Related functions:**

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

