
.. _mat3:

mat3 - 3x3 matrix
-----------------

A :class:`mat3` represents a 3x3 matrix which can be used to store linear
transformations (if you want to store translations or perspective
transformations, you have to use a :class:`mat4`). You can construct a
:class:`mat3` in several ways::

   # all components are set to zero
   M = mat3()

   [   0.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000]

   # identity matrix
   M = mat3(1.0)

   [   1.0000,    0.0000,    0.0000]
   [   0.0000,    1.0000,    0.0000]
   [   0.0000,    0.0000,    1.0000]

   # The elements on the diagonal are set to 2.5
   M = mat3(2.5)

   [   2.5000,    0.0000,    0.0000]
   [   0.0000,    2.5000,    0.0000]
   [   0.0000,    0.0000,    2.5000]

   # All elements are explicitly set (values must be given in row-major order)
   M = mat3(a,b,c,d,e,f,g,h,i)
   M = mat3([a,b,c,d,e,f,g,h,i])

   [ a, b, c]
   [ d, e, f]
   [ g, h, i]

   # Create a copy of matrix N (which also has to be a mat3)
   M = mat3(N)

   # Specify the 3 columns of the matrix (as vec3's or sequences with 3 elements)
   M = mat3(a,b,c)

   [ a[0], b[0], c[0] ]
   [ a[1], b[1], c[1] ]
   [ a[2], b[2], c[2] ]

   # All elements are explicitly set and are stored inside a string
   M = mat3("1,2,3,4,5,6,7,8,9")

   [   1.0000,    2.0000,    3.0000]
   [   4.0000,    5.0000,    6.0000]
   [   7.0000,    8.0000,    9.0000]

**Mathematical operations:**

The mathematical operators are supported with the following combination of
types::

   mat3  =  mat3 + mat3
   mat3  =  mat3 - mat3
   mat3  =  mat3 * mat3
   vec3  =  mat3 * vec3
   vec3  =  vec3 * mat3
   mat3  = float * mat3
   mat3  =  mat3 * float
   mat3  =  mat3 / float
   mat3  =  mat3 % float     # each component
   mat3  = -mat3
   vec3  =  mat3[i]          # get or set column i (as vec3)
   float =  mat3[i,j]        # get or set element in row i and column j

Additionally, you can compare matrices with ``==`` and ``!=``.


.. class:: mat3([args])


   .. method:: mat3.getColumn(index)

      Return column index (0-based) as a :class:`vec3`.


   .. method:: mat3.setColumn(index, value)

      Set column index (0-based) to *value* which has to be a sequence of 3 floats
      (this includes :class:`vec3`).


   .. method:: mat3.getRow(index)

      Return row index (0-based) as a :class:`vec3`.


   .. method:: mat3.setRow(index, value)

      Set row index (0-based) to *value* which has to be a sequence of 3 floats (this
      includes :class:`vec3`).


   .. method:: mat3.getDiag()

      Return the diagonal as a :class:`vec3`.


   .. method:: mat3.setDiag(value)

      Set the diagonal to *value* which has to be a sequence of 3 floats (this
      includes :class:`vec3`).


   .. method:: mat3.toList(rowmajor=False)

      Returns a list containing the matrix elements. By default, the list is in
      column-major order. If you set the optional argument *rowmajor* to ``True``,
      you'll get the list in row-major order.


   .. method:: mat3.identity()

      Returns the identity matrix. This is a static method.


   .. method:: mat3.transpose()

      Returns the transpose of the matrix.


   .. method:: mat3.determinant()

      Returns the determinant of the matrix.


   .. method:: mat3.inverse()

      Returns the inverse of the matrix.


   .. method:: mat3.scaling(s)

      Returns a scaling transformation. The scaling vector *s* must be a 3-sequence
      (e.g. a :class:`vec3`).

          .. math::
       
             \left( \begin{array}{ccc}
             s.x & 0 & 0 \\
             0 & s.y & 0 \\
             0 & 0 & s.z \\
             \end{array} \right)

      This is a static method.


   .. method:: mat3.rotation(angle, axis)

      Returns a rotation transformation. The angle must be given in radians, *axis*
      has to be a 3-sequence (e.g. a :class:`vec3`). ---  This is a static method.


   .. method:: mat3.scale(s)

      Concatenates a scaling transformation and returns *self*. The scaling vector s
      must be a 3-sequence (e.g. a :class:`vec3`).


   .. method:: mat3.rotate(angle, axis)

      Concatenates a rotation transformation and returns *self*. The angle must be
      given in radians, axis has to be a 3-sequence (e.g. a :class:`vec3`).


   .. method:: mat3.ortho()

      Returns a matrix with orthogonal base vectors.


   .. method:: mat3.decompose()

      Decomposes the matrix into a rotation and scaling part. The method returns a
      tuple (rotation, scaling). The scaling part is given as a :class:`vec3`, the
      rotation is still a :class:`mat3`.


   .. method:: mat3.fromEulerXYZ(x, y, z)

      Returns a rotation matrix created from Euler angles. *x* is the angle around the
      x axis, *y* the angle around the y axis and *z* the angle around the z axis. All
      angles must be given in radians. The order of the individual rotations is X-Y-Z
      (where each axis refers to the  local axis, i.e. the first rotation is about the
      x axis which rotates the Y and Z axis, then the second rotation is about the
      rotated Y axis  and so on). ---  This is a static method.


   .. method:: mat3.fromEulerYZX(x, y, z)

      See above. The order is YZX. This is a static method.


   .. method:: mat3.fromEulerZXY(x, y, z)

      See above. The order is ZXY. This is a static method.


   .. method:: mat3.fromEulerXZY(x, y, z)

      See above. The order is XZY. This is a static method.


   .. method:: mat3.fromEulerYXZ(x, y, z)

      See above. The order is YXZ. This is a static method.


   .. method:: mat3.fromEulerZYX(x, y, z)

      See above. The order is ZYX. This is a static method.


   .. method:: mat3.toEulerXYZ()

      Return the Euler angles of a rotation matrix. The order is XYZ.


   .. method:: mat3.toEulerYZX()

      Return the Euler angles of a rotation matrix. The order is YZX.


   .. method:: mat3.toEulerZXY()

      Return the Euler angles of a rotation matrix. The order is ZXY.


   .. method:: mat3.toEulerXZY()

      Return the Euler angles of a rotation matrix. The order is XZY.


   .. method:: mat3.toEulerYXZ()

      Return the Euler angles of a rotation matrix. The order is YXZ.


   .. method:: mat3.toEulerZYX()

      Return the Euler angles of a rotation matrix. The order is ZYX.


   .. method:: mat3.fromToRotation(_from, to)

      Returns a rotation matrix that rotates one vector into another. The generated
      rotation matrix will rotate the vector *_from* into the vector *to*. *_from* and
      *to* must be unit vectors!

      This method is based on the code from:

       | Tomas Möller and John Hughes
       | *Efficiently Building a Matrix to Rotate One Vector to Another*
       | Journal of Graphics Tools, 4(4):1-4, 1999
       | `<http://jgt.akpeters.com/papers/MollerHughes99/>`_

      This is a static method.

