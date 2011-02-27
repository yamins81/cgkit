.. % ValueTable component


:class:`ValueTable` --- Animate a value from a table of values
==============================================================

A :class:`ValueTable` stores time/value pairs and has an output slot that holds
the appropriate value for the current time. The type of the value can be
specified in the constructor. The name of the output slot is always
*output_slot*.


.. class:: ValueTable(name = "ValueTable",  type = "vec3",  values = [],  modulo = None,  tscale = 1.0,  auto_insert = True)

   *type* is the type of the values stored in the component. This is also the type
   of the output slot.

   *values* is a list of initial time/value pairs. Each item must be a 2-tuple
   (time, value).

   *modulo* is the duration of the loop. The animation sequence will be replayed
   after *modulo* seconds. The sequence will not repeat if *modulo* is ``None``.

   *tscale* is a scaling factor for the time. Values smaller than 1.0 will slow
   down the animation.


.. method:: ValueTable.add(t, v)

   Add a new time/value pair to the table. The value *v* must be of the appropriate
   type.

The values can either be added using the :meth:`add` method or using the index
operator. If you want to retrieve the value for a particular time (without using
the value slot) then you can either use the call operator or the index
operator::

   >>> vt=ValueTable(type="double")
   >>> vt.add(0.0, 1.0)
   >>> vt[0.5] = 2.0
   >>> list(vt)
   [(0.0, 1.0), (0.5, 2.0)]
   >>> vt(0.2)
   1.0
   >>> vt[1.5]
   2.0

You can iterate over a :class:`ValueTable` to get the time/value pairs::

   >>> vt=ValueTable(type="double")
   >>> vt.add(0, 1)
   >>> vt.add(1.5, -2)
   >>> vt.add(1.0, 0.5)
   >>> for t,v in vt: print t,v
   ...
   0 1
   1.0 0.5
   1.5 -2

