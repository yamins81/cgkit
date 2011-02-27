
.. _vec4:

vec4 - 4d vector
----------------

A :class:`vec4` represents a 4D vector type. You can construct a :class:`vec4`
by several ways::

   # all components are set to zero
   v = vec4()

   -> (0.0000, 0.0000, 0.0000, 0.0000)

   # set all components to one value
   v = vec4(2.5)

   -> (2.5000, 2.5000, 2.5000, 2.5000)

   # set a 2d vector, the ramaining components will be zero
   v = vec4(1.5, 0.8)

   -> (1.5000, 0.8000, 0.0000, 0.0000)

   # set a 3d vector, the ramaining component will be zero
   v = vec4(1.5, 0.8, -0.5)

   -> (1.5000, 0.8000, -0.5000, 0.0000)

   # set all components
   v = vec4(1.5, 0.8, -0.5, 0.2)

   -> (1.5000, 0.8000, -0.5000, 0.2000)

Additionally you can use all of the above, but store the values inside a tuple,
a list or a string::

   v = vec4([1.5, 0.8, -0.5])
   w = vec4("1.5, 0.8")

Finally, you can initialize a vector with a copy of another vector::

   v = vec4(w)

A :class:`vec4` can be used just like a list with 4 elements, so you can read
and write components using the index operator or by accessing the components by
name::

   >>> v=vec4(1,2,3,1)
   >>> print v[0]
   1.0
   >>> print v.y
   2.0
   >>> print v.w
   1.0
   >>> print v.t   # this is the same as v.w
   1.0

The 4th component can be accessed either by the name ``w`` or ``t``. You might
prefer the former name when using the vector as a homogenous coordinate while
the latter might be preferable when the 4th component shall represent a time
value.

**Mathematical operations:**

The mathematical operators are supported with the following combination of
types::

   vec4  =  vec4 + vec4
   vec4  =  vec4 - vec4
   float =  vec4 * vec4      # dot product
   vec4  = float * vec4
   vec4  =  vec4 * float
   vec4  =  vec4 / float
   vec4  =  vec4 % float     # each component
   vec4  =  vec4 % vec4      # component wise
   vec4  = -vec4
   float =  vec4[i]          # get or set element

Additionally, you can compare vectors with ``==``, ``!=``, ``<``,  ``<=``,
``>``, ``>=``. Each comparison (except ``<`` and ``>``) takes an epsilon
environment into account, this means two values are considered to be equal if
their absolute difference is less than or equal to a threshold value epsilon.
You can read and write this threshold value using the functions
:func:`getEpsilon` and :func:`setEpsilon`.

Taking the absolute value of a vector will return the length of the vector::

   float = abs(v)            # this is equivalent to v.length()


.. class:: vec4([args])


   .. method:: vec4.length()

      Returns the length of the vector (:math:`\sqrt{x^2+y^2+z^2+w^2}`). This is
      equivalent to calling ``abs(self)``.


   .. method:: vec4.normalize()

      Returns normalized vector. If the method is called on the null vector (where
      each component is zero) a :exc:`ZeroDivisionError` is raised.


   .. method:: vec4.min()

      Returns the minimum value of the components.


   .. method:: vec4.max()

      Returns the maximum value of the components.


   .. method:: vec4.minIndex()

      Return the index of the component with the minimum value.


   .. method:: vec4.maxIndex()

      Return the index of the component with the maximum value.


   .. method:: vec4.minAbs()

      Returns the minimum absolute value of the components.


   .. method:: vec4.maxAbs()

      Returns the maximum absolute value of the components.


   .. method:: vec4.minAbsIndex()

      Return the index of the component with the minimum absolute value.


   .. method:: vec4.maxAbsIndex()

      Return the index of the component with the maximum absolute value.

