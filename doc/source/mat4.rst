
.. _mat4:

mat4 - 4x4 matrix
-----------------

A :class:`mat4` represents a 4x4 matrix which can be used to store
transformations. You can construct a :class:`mat4` in several ways::

   # all components are set to zero
   M = mat4()

   [   0.0000,    0.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000,    0.0000]

   # identity matrix
   M = mat4(1.0)

   [   1.0000,    0.0000,    0.0000,    0.0000]
   [   0.0000,    1.0000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    1.0000,    0.0000]
   [   0.0000,    0.0000,    0.0000,    1.0000]

   # The elements on the diagonal are set to 2.5
   M = mat4(2.5)

   [   2.5000,    0.0000,    0.0000,    0.0000]
   [   0.0000,    2.5000,    0.0000,    0.0000]
   [   0.0000,    0.0000,    2.5000,    0.0000]
   [   0.0000,    0.0000,    0.0000,    2.5000]

   # All elements are explicitly set (values must be given in row-major order)
   M = mat4(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p)
   M = mat4([a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p])

   [ a, b, c, d ]
   [ e, f, g, h ]
   [ i, j, k, l ]
   [ m, n, o, p ]

   # Create a copy of matrix N (which also has to be a mat4)
   M = mat4(N)

   # Specify the 4 columns of the matrix (as vec4's or sequences with 4 elements)
   M = mat4(a,b,c,d)

   [ a[0], b[0], c[0], d[0] ]
   [ a[1], b[1], c[1], d[1] ]
   [ a[2], b[2], c[2], d[2] ]
   [ a[3], b[3], c[3], d[3] ]

   # All elements are explicitly set and are stored inside a string
   M = mat4("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16")

   [   1.0000,    2.0000,    3.0000,    4.0000]
   [   5.0000,    6.0000,    7.0000,    8.0000]
   [   9.0000,   10.0000,   11.0000,   12.0000]
   [  13.0000,   14.0000,   15.0000,   16.0000]

**Mathematical operations:**

The mathematical operators are supported with the following combination of
types::

   mat4  =  mat4 + mat4
   mat4  =  mat4 - mat4
   mat4  =  mat4 * mat4
   vec4  =  mat4 * vec4
   vec4  =  vec4 * mat4
   vec3  =  mat4 * vec3      # missing coordinate in vec3 is implicitly set to 1.0
   vec3  =  vec3 * mat4      # missing coordinate in vec3 is implicitly set to 1.0
   mat4  = float * mat4
   mat4  =  mat4 * float
   mat4  =  mat4 / float
   mat4  =  mat4 % float     # each component
   mat4  = -mat4
   vec4  =  mat4[i]          # get or set column i (as vec4)
   float =  mat4[i,j]        # get or set element in row i and column j

Additionally, you can compare matrices with ``==`` and ``!=``.


.. class:: mat4([args])


   .. method:: mat4.getColumn(index)

      Return column index (0-based) as a :class:`vec4`.


   .. method:: mat4.setColumn(index, value)

      Set column index (0-based) to *value* which has to be a sequence of 4 floats
      (this includes :class:`vec4`).


   .. method:: mat4.getRow(index)

      Return row index (0-based) as a :class:`vec4`.


   .. method:: mat4.setRow(index, value)

      Set row index (0-based) to *value* which has to be a sequence of 4 floats (this
      includes :class:`vec4`).


   .. method:: mat4.getDiag()

      Return the diagonal as a :class:`vec4`.


   .. method:: mat4.setDiag(value)

      Set the diagonal to *value* which has to be a sequence of 4 floats (this
      includes :class:`vec4`).


   .. method:: mat4.toList(rowmajor=False)

      Returns a list containing the matrix elements. By default, the list is in
      column-major order (which can be directly used in OpenGL or RenderMan).  If you
      set the optional argument *rowmajor* to ``True``, you will get the list in row-
      major order.


   .. method:: mat4.identity()

      Returns the identity matrix. This is a static method.


   .. method:: mat4.transpose()

      Returns the transpose of the matrix.


   .. method:: mat4.determinant()

      Returns the determinant of the matrix.


   .. method:: mat4.inverse()

      Returns the inverse of the matrix.


   .. method:: mat4.translation(t)

      Returns a translation transformation. The translation vector t must be a
      3-sequence (e.g. a vec3).

          .. math::
          
             \left( \begin{array}{cccc}
             1 & 0 & 0 & t.x \\
             0 & 1 & 0 & t.x \\
             0 & 0 & 1 & t.x \\
             0 & 0 & 0 & 1 
             \end{array} \right)

      This is a static method.


   .. method:: mat4.scaling(s)

      Returns a scaling transformation. The scaling vector *s* must be a 3-sequence
      (e.g. a :class:`vec3`).

          .. math::
          
             \left( \begin{array}{cccc}
             s.x & 0 & 0 & 0\\
             0 & s.y & 0 & 0\\
             0 & 0 & s.z & 0\\
             0 & 0 & 0 & 1\\
             \end{array} \right)

      This is a static method.


   .. method:: mat4.rotation(angle, axis)

      Returns a rotation transformation. The angle must be given in radians, *axis*
      has to be a 3-sequence (e.g. a :class:`vec3`). ---  This is a static method.


   .. method:: mat4.translate(t)

      Concatenate a translation transformation and return *self*. The translation
      vector *t* must be a 3-sequence (e.g. a :class:`vec3`).


   .. method:: mat4.scale(s)

      Concatenates a scaling transformation and returns *self*. The scaling vector s
      must be a 3-sequence (e.g. a :class:`vec3`).


   .. method:: mat4.rotate(angle, axis)

      Concatenates a rotation transformation and returns *self*. The angle must be
      given in radians, axis has to be a 3-sequence (e.g. a :class:`vec3`).


   .. method:: mat4.frustum(left, right, bottom, top, near, far)

      Returns a matrix that represents a perspective depth transformation. This method
      is equivalent to the OpenGL command :cfunc:`glFrustum`. ---  This is a static
      method.

      .. note::

         If you want to use this transformation in RenderMan, keep in mind that the
         RenderMan camera is looking in the positive z direction whereas the OpenGL
         camera is looking in the negative direction.


   .. method:: mat4.perspective(fovy, aspect, near, far)

      Similarly to :meth:`frustum` this method returns a perspective transformation,
      the only difference is the meaning of the input parameters. The method is
      equivalent to the OpenGL command :cfunc:`gluPerspective`. ---  This is a static
      method.


   .. method:: mat4.orthographic(left, right, bottom, top, near, far)

      Returns a matrix that represents an orthographic transformation. This method is
      equivalent to the OpenGL command :cfunc:`glOrtho`. ---  This is a static method.


   .. method:: mat4.lookAt(pos, target, up=(0,0,1))

      Returns a transformation that moves the coordinate system to position *pos* and
      rotates it so that the z-axis points onto point *target*. The y-axis is pointing
      as closely as possible into the same direction as *up*. All three parameters
      have to be a 3-sequence.

      You can use this transformation to position objects in the scene that have to be
      oriented towards a particular point. Or you can use it to position the camera in
      the virtual world. In this case you have to take the inverse of this
      transformation as viewport transformation (to convert world space coordinates
      into camera space coordinates).

      This is a static method.


   .. method:: mat4.ortho()

      Returns a matrix with orthogonal base vectors. The x-, y- and z-axis are made
      orthogonal. The fourth column and row remain untouched.


   .. method:: mat4.decompose()

      Decomposes the matrix into a translation, rotation and scaling. The method
      returns a tuple (translation, rotation, scaling). The translation and scaling
      parts are given as :class:`vec3`, the rotation is still given as a
      :class:`mat4`.


   .. method:: mat4.getMat3()

      Returns a :class:`mat3` which is a copy of self without the 4th column and row.


   .. method:: mat4.setMat3(m3)

      Sets the first three columns and rows to the values in *m3*.

