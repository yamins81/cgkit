.. % Expression component


:class:`Expression` --- Using expressions to drive animations
=============================================================

An :class:`Expression` outputs a single value that is driven by a user defined
expression. The expression is specified by a string and can use an arbitrary
number of parameters. The parameters and their default values have to be
provided to the constructor via keyword arguments. An exception is the special
variable ``t`` which will always hold the current time (unless you declare it
explicitly).  For each parameter a slot is created (``<name>_slot``), so it is
also possible to animate the parameters. The output value can be accessed via
the ``output`` and ``output_slot`` attributes.


.. class:: Expression(expr = "",  exprtype = None,  name = "Expression",  **parameters)

   *expr* is the expression (in Python syntax) that is used to compute the output
   values. It may use mathematical functions defined in the :mod:`math` and
   :mod:`sl` modules.

   *exprtype* is the return type of the expression (``float``,  ``vec3``, ...). If
   this parameter is not given, the component tries to figure out the return type
   itself by evaluating the expression using the default values and inspecting the
   return type. If the expression returns a tuple the output type is automatically
   set to vec3, vec4, mat3 or mat4 (depending on the number of elements).

   *name* is the component's name.

   Any additional keyword argument is a parameter used in the expression. Every
   parameter used in the expression has to be declared except the variable ``t``
   which will automatically receive the current time value. If you declare ``t``
   yourself, it will be just an ordinary variable.

Example::

   s = Sphere()
   e = Expression("1.0 + amp*sin(freq*t)", amp=0.2, freq=2.0)
   e.output_slot.connect(s.radius_slot)

When the above expression is created, it will have the following slots:

* ``output_slot``

* ``amp_slot``

* ``freq_slot``

* ``t_slot``

