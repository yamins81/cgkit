
:mod:`events` --- Standard event names and classes
==================================================

.. module:: cgkit.events
   :synopsis: Standard event names and classes


This module defines the names of the standard events and the corresponding event
classes.

The actual event name is a capitalized version of the symbol where all
underscores are removed (for example, ``STEP_FRAME = "StepFrame"``).


.. data:: STEP_FRAME

   This event is created whenever the timer is stepped forward one frame using
   ``timer.step()``. The event takes no arguments.


.. data:: RESET

   This event is created to reset the simulation/animation. This effectively sets
   the time back to 0. Any component that has an internal state should connect to
   this method and reset its state whenever the event is signalled.


.. data:: KEY_PRESS

   A key was pressed on the keyboard. The callback function receives  a
   :class:`KeyEvent` object as argument.


.. data:: KEY_RELEASE

   A key was released on the keyboard. The callback function receives  a
   :class:`KeyEvent` object as argument.


.. data:: LEFT_DOWN

   The left mouse button was pressed. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: LEFT_UP

   The left mouse button was released. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: MIDDLE_DOWN

   The middle mouse button was pressed. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: MIDDLE_UP

   The middle mouse button was released. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: RIGHT_DOWN

   The right mouse button was pressed. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: RIGHT_UP

   The right mouse button was released. The callback function receives  a
   :class:`MouseButtonEvent` object as argument.


.. data:: MOUSE_BUTTON_DOWN

   A mouse button other than the left, middle or right button was pressed. The
   callback function receives a :class:`MouseButtonEvent` object as argument.


.. data:: MOUSE_BUTTON_UP

   A mouse button other than the left, middle or right button was released. The
   callback function receives a :class:`MouseButtonEvent` object as argument.


.. data:: MOUSE_MOVE

   The mouse was moved. The callback function receives a :class:`MouseMoveEvent`
   object as argument.


.. data:: MOUSE_WHEEL

   The mouse wheel was rotated. The callback function receives a
   :class:`MouseWheelEvent` object as argument.


.. data:: JOYSTICK_AXIS


.. data:: JOYSTICK_BALL


.. data:: JOYSTICK_HAT


.. data:: JOYSTICK_BUTTON_DOWN


.. data:: JOYSTICK_BUTTON_UP


.. data:: SPACE_MOTION

   A SpaceMouse/SpaceBall was moved or rotated. The callback function receives a
   :class:`SpaceMotionEvent` object as argument.


.. data:: SPACE_BUTTON_DOWN

   A SpaceMouse/SpaceBall button was pressed. The callback function receives a
   :class:`SpaceButtonEvent` object as argument.


.. data:: SPACE_BUTTON_UP

   A SpaceMouse/SpaceBall button was released. The callback function receives a
   :class:`SpaceButtonEvent` object as argument.


.. data:: SPACE_BUTTON_ZERO

   The user stopped moving/rotating the SpaceMouse/SpaceBall. The event takes no
   arguments.


.. data:: TABLET

   A tablet (or similar pointing device) generated an event.

.. note::

   It actually depends on the application used to process a scene if a particular
   type of event occurs or not. For example, events like a key press or mouse move
   do occur in the interactive viewer tool, but not in the offline rendering tool.

.. % -----------


KeyEvent object
---------------

The :class:`KeyEvent` class is passed as argument to the ``KEY_PRESS`` and
``KEY_RELEASE`` events. The class contains the attributes ``key``, ``keycode``
and ``mods`` (see the constructor for a description).


.. class:: KeyEvent(key, keycode, mods=0)

   *key* is a unicode string that contains the key that was  pressed or released.
   The key is the translated key which means that it may depend on other keys (such
   as Shift) or previously pressed keys. For example, if you just press the 'a' key
   you receive a lower case 'a', but if you press Shift and 'a' then you will get
   an upper case 'A'.

   *keycode* is the code of the untranslated key that was pressed or released. The
   values for common keys are defined in the :mod:`keydefs` module (i.e.
   ``KEY_LEFT``, ``KEY_UP``, etc.).

   *mods* contains the modifier flags which is a combination of ``KEYMOD_SHIFT``,
   ``KEYMOD_CONTROL``, ``KEYMOD_ALT`` and ``KEYMOD_META``. The respective flag is
   set if the Shift, Control, Alt or Meta key was pressed while the event key was
   pressed or released.


.. method:: KeyEvent.shiftKey()

   Return ``True`` if the key is a Shift key (no matter if it is the left or right
   key).


.. method:: KeyEvent.controlKey()

   Return ``True`` if the key is a Control key (no matter if it is the left or
   right key).


.. method:: KeyEvent.altKey()

   Return ``True`` if the key is an Alt key (no matter if it is the left or right
   key).

.. % -----------


MouseButtonEvent object
-----------------------

The :class:`MouseButtonEvent` class is passed as argument whenever a mouse
button was pressed or released.


.. class:: MouseButtonEvent(button, x, y, x0, y0)

   *button* is the mouse button number (1 = left button, 2 = middle button, 3 =
   right button).

   *x*/*y* is the pixel position of the mouse where 0/0 is the upper left corner.

   *x0*/*y0* is the normalized position of the mouse where each  component lies in
   the range [0,1).

.. % -----------


MouseWheelEvent object
----------------------

The :class:`MouseWheelEvent` class is passed as argument whenever the mouse
wheel is rotated.


.. class:: MouseWheelEvent(delta, x, y, x0, y0)

   *delta* is the wheel delta (usually 120 if the wheel was rotated forward and
   -120 if the wheel was rotated backward).

   *x*/*y* is the pixel position of the mouse where 0/0 is the upper left corner.

   *x0*/*y0* is the normalized position of the mouse where each  component lies in
   the range [0,1).

.. % -----------


MouseMoveEvent object
---------------------

The :class:`MouseMoveEvent` class is passed as argument whenever the mouse is
moved.


.. class:: MouseMoveEvent(x, y, dx, dy, x0, y0, dx0, dy0, buttons)

   *x*/*y* is the pixel position of the mouse where 0/0 is the upper left corner.

   *dx*/*dy* is the mouse delta, i.e. the distance travelled since the last event.

   *x0*/*y0* is the normalized position of the mouse where each  component lies in
   the range [0,1).

   *dx0*/*dy0* is the normalized mouse delta.

   *buttons* contains the mouse buttons that are currently pressed. Each bit
   corresponds to a mouse button (bit 0 = left button, bit 1 = middle button,
   etc.).

.. % -----------


JoystickAxisEvent object
------------------------


.. class:: JoystickAxisEvent(joystick, axis, value)


JoystickHatEvent object
-----------------------


.. class:: JoystickHatEvent(joystick, hat, x, y)


JoystickBallEvent object
------------------------


.. class:: JoystickBallEvent(joystick, ball, value)


JoystickButtonEvent object
--------------------------


.. class:: JoystickButtonEvent(joystick, button)

.. % -----------


SpaceMotionEvent object
-----------------------


.. class:: SpaceMotionEvent(translation, rotation, period)

   *translation* is a :class:`vec3` containing the curreent translation of the
   space mouse. The coordinate system of the space mouse is left handed and such
   that the X axis points to the right, the Y axis upwards and the Z axis into the
   screen.

   *rotation* is a :class:`vec3` containing the current rotation of the space
   mouse. The vector is the rotation axis and the magnitude represents the angle.

   *period* is the time in milliseconds since the last device event.


SpaceButtonEvent object
-----------------------


.. class:: SpaceButtonEvent(button)

