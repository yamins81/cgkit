:mod:`sl` --- RenderMan Shading Language functionality
======================================================

.. module:: cgkit.sl
   :synopsis: Provides the same functions than the RenderMan Shading Language


This module provides some of the functionality from the RenderMan Shading
Language. Those functions that require an actual rendering context (surfaces,
light sources, etc.) are not supported.

Most of the functions can be used just like in the Shading Language. An
exception are those functions whose return type is dependant on the context as
it is the case with :func:`random` or :func:`noise`. Here, those functions have
to be prepended with the  return type, for example :func:`float_random` or
:func:`point_noise` (that is, the cast is part of the name).


Constants
---------


.. data:: PI

   This is the same as ``math.pi``.


Math functions
--------------


.. function:: abs(x)

   Returns the absolute value of its argument. This is just the builtin :func:`abs`
   function.


.. function:: acos(a)

   Returns the arc cosine. This function is imported from the :mod:`math` module.


.. function:: asin(a)

   Returns the arc sine. This function is imported from the :mod:`math` module.


.. function:: atan(y[, x])

   Returns the arc tangent. With one argument :func:`math.atan` is called, with two
   arguments :func:`math.atan2` is called.


.. function:: ceil(x)

   Returns the largest integer not greater than *x*. This function is imported from
   the :mod:`math` module.


.. function:: clamp(x, min, max)

   Returns *min* if *x* < *min*, *max* if *x* > *max*,  otherwise *x*.


.. function:: color_cellnoise(p)

   Returns a color value (actually a :class:`vec3`) whose value is a (pseudo)
   random function of its arguments. The return value is constant between integer
   lattice points.


.. function:: color_noise(p)

   Returns a color value (actually a :class:`vec3`) whose value is a (pseudo)
   random function of its arguments.


.. function:: color_pnoise(p, period)

   Returns a color value (actually a :class:`vec3`) whose value is a periodic
   (pseudo) random function of its arguments.


.. function:: color_random()

   Return a color whose componenets are a random number between 0 and 1. The
   function actually returns a :class:`vec3`.


.. function:: cos(a)

   Returns the cosine. This function is imported from the :mod:`math` module.


.. function:: degrees(rad)

   Converts from radians to degrees.


.. function:: exp(x)

   Returns ``pow(e,x)``. This function is imported from the :mod:`math` module.


.. function:: float_cellnoise(p)

   Returns a float value which is a (pseudo) random function of its arguments. The
   return value is constant between integer lattice points. This function is
   imported from the :mod:`noise` module.


.. function:: float_noise(p)

   Returns a float value which is a (pseudo) random function of its arguments. This
   function is imported from the :mod:`noise` module.


.. function:: float_pnoise(p, period)

   Returns a float value which is a periodic (pseudo) random function of its
   arguments. This function is imported from the :mod:`noise` module.


.. function:: float_random()

   Return a random number between 0 and 1. This call is equivalent to
   :func:`random.random`.


.. function:: floor(x)

   Returns the smallest integer not smaller than *x*. This function is imported
   from the :mod:`math` module.


.. function:: inversesqrt(x)

   Returns ``1/sqrt(x)``.


.. function:: log(x[, base])

   Returns the natural logarithm of *x* (the same as :func:`math.log`) or the
   logarithm to the specified base.


.. function:: max(a, b, ...)

   Returns the argument with maximum value. This is just the builtin :func:`max`
   function.


.. function:: min(a, b, ...)

   Returns the argument with minimum value. This is just the builtin  :func:`min`
   function.


.. function:: mix(val0, val1, t)

   For *t* = 0 the value *val0* is returned, for *t* = 1 the value  *val1* is returned.
   For values of *t* between 0 and 1 a linearly interpolated value is returned.


.. function:: mod(a, b)

   Returns ``a%b``. This is just an equivalent for the %-operator.


.. function:: point_cellnoise(p)

   Returns a point (as a :class:`vec3`) whose value is a (pseudo) random function
   of its arguments. The return value is constant between integer lattice points.


.. function:: point_noise(p)

   Returns a point (as a :class:`vec3`) whose value is a (pseudo) random function
   of its arguments.


.. function:: point_pnoise(p, period)

   Returns a point (as a :class:`vec3`) whose value is a periodic (pseudo) random
   function of its arguments.


.. function:: point_random()

   Return a point (a :class:`vec3`) whose componenets are a random number between 0
   and 1.


.. function:: pow(x, y)

   Returns x\*\*y. This function is imported from the :mod:`math` module.


.. function:: radians(deg)

   Converts from degrees to radians.


.. function:: round(x)

   Returns the integer closest to *x*. This is just the builtin  :func:`round`
   function.


.. function:: sign(x)

   Returns -1 with a negative argument, +1 with a positive argument, and 0 if its
   argument is zero.


.. function:: sin(a)

   Returns the sine. This function is imported from the :mod:`math` module.


.. function:: smoothstep(min, max, value)

   Returns 0 if *value* < *min*, 1 if *value* > *max*, and performs a smooth
   Hermite interpolation between 0 and 1 in the interval *min* to *max*.


.. function:: spline(t, controlpoints)

   Fits a spline to the control points given and returns the value at *t* which
   ranges from 0 to 1. At least four control points must always be given.


.. function:: sqrt(x)

   Returns the square root. This function is imported from the :mod:`math` module.


.. function:: step(min, x)

   Returns 0 if *x* < *min*, otherwise 1.


.. function:: tan(a)

   Returns the tangent. This function is imported from the :mod:`math` module.


.. function:: tan(a)

   Returns the tangent. This function is imported from the :mod:`math` module.


.. function:: vector_cellnoise(p)

   Returns a vector (as a :class:`vec3`) whose value is a (pseudo) random function
   of its arguments. The return value is constant between integer lattice points.


.. function:: vector_noise(p)

   Returns a vector (as a :class:`vec3`) whose value is a (pseudo) random function
   of its arguments.


.. function:: vector_pnoise(p, period)

   Returns a vector (as a :class:`vec3`) whose value is a periodic (pseudo) random
   function of its arguments.


Geometric functions
-------------------


.. function:: distance(p1, p2)

   Returns the distance between two points. The arguments should be of type
   :class:`vec3`.


.. function:: faceforward(N, I, Nref)

   Flips *N* so that it faces in the direction opposite to *I*.

   .. note::

      In contrast to the Shading Language *Nref* is not optional.


.. function:: length(v)

   Returns the length of a vector. This is equivalent to calling  :meth:`v.length`.


.. function:: normalize(v)

   Returns a unit vector in the direction of *v*. This is equivalent to calling
   :meth:`v.normalize`.


.. function:: ptlined(p0, p1, q)

   Returns the distance between point *q* and the line segment *p0*, *p1*. The
   arguments should be of type :class:`vec3`.


.. function:: reflect(I, N)

   Returns the reflection vector given an incident direction *I* and a normal
   vector *N*. This is equivalent to calling :meth:`I.reflect(N)`.


.. function:: refract(I, N, eta)

   Returns the transmitted vector given an incident direction *I*, the normal
   vector *N* and the relative index of refraction *eta*. This is equivalent to
   calling :meth:`I.refract(N, eta)`.


.. function:: xcomp(p)

   Return the x component of *p*. This is equivalent to ``p.x``.


.. function:: ycomp(p)

   Return the y component of *p*. This is equivalent to ``p.y``.


.. function:: zcomp(p)

   Return the z component of *p*. This is equivalent to ``p.z``.


.. function:: setxcomp(p, x)

   Set the x component of *p*. This is equivalent to ``p.x = x``.


.. function:: setycomp(p, y)

   Set the y component of *p*. This is equivalent to ``p.y = y``.


.. function:: setzcomp(p, z)

   Set the z component of *p*. This is equivalent to ``p.z = z``.


.. function:: comp(c, index)

   Get an individual color component. This is equivalent to ``c[index]``.


.. function:: setcomp(c, index, value)

   Set an individual color component. This is equivalent to  ``c[index] = value``.


String functions
----------------


.. function:: concat(str1, ..., strn)

   Returns a concatenated string.


.. function:: format(pattern, val1, val2, ..., valn)

   Returns a formatted string (similar to the C function :cfunc:`sprintf`). Any
   occurance of the character ``%`` followed by a letter is replaced by a value. In
   this implementation it doesn't matter what letter you are actually using (in the
   Shading Language it would be ``%f`` for floats, ``%p`` for points, vectors or normals,
   ``%c`` for colors, ``%m`` for matrices and ``%s`` for strings).


.. function:: match(pattern, subject)

   String pattern matching.


.. function:: printf(pattern, val1, val2, ..., valn)

   Prints the values of the specified variables. Any occurance of the character ``%``
   followed by a letter is replaced by a value. In this implementation it doesn't
   matter what letter you are actually using (in the Shading Language it would be
   ``%f`` for floats, ``%p`` for points, vectors or normals, ``%c`` for colors,
   ``%m`` for matrices and ``%s`` for strings).


Unsupported functions
---------------------

The following functions are not supported by this module:

* :cfunc:`Du`

* :cfunc:`Dv`

* :cfunc:`Deriv`

* :cfunc:`filterstep`

* :cfunc:`area`

* :cfunc:`calculatenormal`

* :cfunc:`depth`

* :cfunc:`fresnel`

* :cfunc:`transform`

* :cfunc:`vtransform`

* :cfunc:`ntransform`

* :cfunc:`ambient`

* :cfunc:`diffuse`

* :cfunc:`phong`

* :cfunc:`specular`

* :cfunc:`specularbrdf`

* :cfunc:`trace`

* :cfunc:`environment`

* :cfunc:`shadow`

* :cfunc:`texture`

* :cfunc:`textureinfo`

* :cfunc:`atmosphere`

* :cfunc:`displacement`

* :cfunc:`incident`

* :cfunc:`lightsource`

* :cfunc:`opposite`

* :cfunc:`surface`

* :cfunc:`attribute`

* :cfunc:`option`

* :cfunc:`rendererinfo`

