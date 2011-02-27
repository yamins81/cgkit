.. _timer:

:class:`Timer` --- Manages the virtual time
===========================================

The :class:`Timer` class manages the virtual time and drives the entire
animation/simulation. Everything that is animated either directly or indirectly
connects to the time slot of the global :class:`Timer` instance. This global
timer is automatically created by the scene and can be obtained by calling the
:meth:`timer` method of the :class:`Scene` class.


.. class:: Timer(name = ''Timer'',  auto_insert = True)

   *name* is the component name.

.. attribute:: Timer.time

   Current virtual time in seconds (``DoubleSlot``). This value is increased by
   :attr:`timestep` whenever the :meth:`step()` method is called.

.. attribute:: Timer.timestep

   The delta time step in seconds that represents one frame.


.. attribute:: Timer.duration

   The total duration in seconds of the animation/simulation.


.. attribute:: Timer.frame

   Current (fractional) frame number (as float).


.. attribute:: Timer.fps

   Current frame rate (as float).


.. attribute:: Timer.framecount

   Total number of frames of the animation/simulation (as float).


.. attribute:: Timer.clock

   This value contains the value in seconds of the real clock.

.. method:: Timer.startClock()

   Start the real clock.


.. method:: Timer.stopClock()

   Stop the real clock.


.. method:: Timer.step()

   Step to the next frame. This call increases the current virtual time by
   :attr:`timestep` and emits a ``STEP_FRAME`` event.

