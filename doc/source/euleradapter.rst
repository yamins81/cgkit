.. % EulerAdapter component


:class:`EulerAdapter` --- Convert euler angles to mat3, mat4 or quat
====================================================================

The :class:`EulerAdapter` class can be used to convert euler angles to either a
:class:`mat3`, a :class:`mat4` or a :class:`quat`. For example, you can plug an
:class:`EulerAdapter` into the position slot of a world object to specify the
orientation of the object via euler angles.


.. class:: EulerAdapter(anglex = 0,  angley = 0, anglez = 0, radians = False, order = "xyz", outtype = "mat3", name = "EulerAdapter", auto_insert = True)

   *anglex*, *angley* and *anglez* are the initial angles around the x-, y- and
   z-axis.

   *radians* determines whether angles are measured in degrees (default) or
   radians.

   *order* is a string with a combination of "x", "y" and "z" and specifies  the
   order of the rotations.

   *outtype* is the type of the output slot and can be one of "mat3", "mat4" and
   "quat".

The attributes *anglex*, *angley* and *anglez* always hold the values of their
corresponding slot. The output value is available as *output* / *output_slot*.

