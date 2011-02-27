.. % MotionPath


:class:`MotionPath` --- Motion path
===================================


.. class:: MotionPath(name = "MotionPath",  curve = None, begintime = 0.0, endtime = 1.0, loop = False, follow = False, bank = False, bankamplitude = 0.1, )

   *curve* is an object supporting the curve protocol.

   *begintime* and *endtime* specify the parameter interval of the motion. At
   *begintime* the object will be located at the beginning of the curve and at
   *endtime* it will be at the end.

   *loop* is a boolean that specifies if the motion is repeated when  outside the
   specified parameter interval.

   *follow* determines if the object will change its orientation to follow the
   path.

   *bank* determines if the object will roll if the curve makes a turn.
   *bankamplitude* specifies how much the obect will roll.

