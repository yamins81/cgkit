
.. _vec3:

vec3 - 3d vector
----------------

A :class:`vec3` represents a 3D vector type that can be used to store points,
vector, normals or even colors. You can construct a :class:`vec3` by several
ways::

   # all components are set to zero
   v = vec3()

   -> (0.0000, 0.0000, 0.0000)

   # set all components to one value
   v = vec3(2.5)

   -> (2.5000, 2.5000, 2.5000)

   # set a 2d vector, the 3rd component will be zero
   v = vec3(1.5, 0.8)

   -> (1.5000, 0.8000, 0.0000)

   # initialize all three components
   v = vec3(1.5, 0.8, -0.5)

   -> (1.5000, 0.8000, -0.5000)

Additionally you can use all of the above, but store the values inside a tuple,
a list or a string::

   v = vec3([1.5, 0.8, -0.5])
   w = vec3("1.5, 0.8")

Finally, you can initialize a vector with a copy of another vector::

   v = vec3(w)

A :class:`vec3` can be used just like a list with 3 elements, so you can read
and write components using the index operator or by accessing the components by
name::

   >>> v=vec3(1,2,3)
   >>> print v[0]
   1.0
   >>> print v.y
   2.0

**Mathematical operations:**

The mathematical operators are supported with the following combination of
types::

   vec3  =  vec3 + vec3
   vec3  =  vec3 - vec3
   float =  vec3 * vec3      # dot product
   vec3  = float * vec3
   vec3  =  vec3 * float
   vec3  =  vec3 / float
   vec3  =  vec3 % float     # each component
   vec3  =  vec3 % vec3      # component wise
   vec3  = -vec3
   float =  vec3[i]          # get or set element

Additionally, you can compare vectors with ``==``, ``!=``, ``<``,  ``<=``,
``>``, ``>=``. Each comparison (except ``<`` and ``>``) takes an epsilon
environment into account, this means two values are considered to be equal if
their absolute difference is less than or equal to a threshold value epsilon.
You can read and write this threshold value using the functions
:func:`getEpsilon` and :func:`setEpsilon`.

Taking the absolute value of a vector will return the length of the vector::

   float = abs(v)            # this is equivalent to v.length()


.. class:: vec3([args])


   .. method:: vec3.angle(other)

      Return angle (in radians) between *self* and *other*.


   .. method:: vec3.cross(other)

      Return cross product of *self* and *other*.


   .. method:: vec3.length()

      Returns the length of the vector (:math:`\sqrt{x^2+y^2+z^2}`). This is
      equivalent to calling ``abs(self)``.


   .. method:: vec3.normalize()

      Returns normalized vector. If the method is called on the null vector (where
      each component is zero) a :exc:`ZeroDivisionError` is raised.


   .. method:: vec3.reflect(N)

      Returns the reflection vector. *N* is the surface normal which has to be of unit
      length.


   .. method:: vec3.refract(N, eta)

      Returns the transmitted vector. *N* is the surface normal which has to be of
      unit length. *eta* is the relative index of refraction. If the returned vector
      is zero then there is no transmitted light because of total internal reflection.


   .. method:: vec3.ortho()

      Returns a vector that is orthogonal to *self* (where ``self*self.ortho()==0``).


   .. method:: vec3.min()

      Returns the minimum value of the components.


   .. method:: vec3.max()

      Returns the maximum value of the components.


   .. method:: vec3.minIndex()

      Return the index of the component with the minimum value.


   .. method:: vec3.maxIndex()

      Return the index of the component with the maximum value.


   .. method:: vec3.minAbs()

      Returns the minimum absolute value of the components.


   .. method:: vec3.maxAbs()

      Returns the maximum absolute value of the components.


   .. method:: vec3.minAbsIndex()

      Return the index of the component with the minimum absolute value.


   .. method:: vec3.maxAbsIndex()

      Return the index of the component with the maximum absolute value.

