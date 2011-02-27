.. _joystick:

:mod:`joystick` --- Joystick support
====================================

.. module:: cgkit.joystick
   :synopsis: Joystick support

The :class:`Joystick` class represents a joystick device. It can be used to
retrieve the current values of the joystick and to calibrate the joystick axes.
The minimum and maximum positions of the axes are automatically determined while
the joystick is in use. The middle position has to be set manually by calling
the :meth:`setAxisMiddle` method.


.. class:: Joystick

   The scene will contain one :class:`Joystick` object for every joystick that is
   found.


.. attribute:: Joystick.id

   The device id of the joystick.


.. attribute:: Joystick.name

   The name of the joystick device.


.. attribute:: Joystick.numaxes

   The number of axes this joystick has.


.. attribute:: Joystick.numhats

   The number of hats this joystick has.


.. attribute:: Joystick.numballs

   The number of balls this joystick has.


.. attribute:: Joystick.numbuttons

   The number of buttons this joystick has.


.. method:: Joystick.getAxis(axis)

   Returns a calibrated axis value as a float. *axis* is the number of the axis
   (starting with 0).


.. method:: Joystick.getHat(hat)

   Returns a hat value. *hat* is the number of the hat (starting with 0). The
   return value is a 2-tuple of integers (x, y).


.. method:: Joystick.getBall(ball)

   Returns a ball value as a float. *ball* is the number of the ball (starting with
   0).


.. method:: Joystick.getButton(button)

   Return the state of button *button* (0-based number) as a boolean.


.. method:: Joystick.setAxis(axis, value)


.. method:: Joystick.setHat(hat, x, y)


.. method:: Joystick.setBall(ball, value)


.. method:: Joystick.setButton(button, value)


.. method:: Joystick.setAxisMiddle(axis=None, mid=None)

   Sets the joystick axis position that will be mapped to the value 0.0.  This
   means, if the joystick outputs the (uncalibrated) value *mid*, then the
   calibrated value will be 0.0. *axis* is the number of the axis you wish to set.
   If it is ``None``, all axes are set at once. *mid* is the middle value. If it is
   ``None``, the current value is used.

