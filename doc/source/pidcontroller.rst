.. % PIDController component


:class:`PIDController` --- Proportional-Integral-Derivative controller
======================================================================

A PID controller is a standard component in industrial control applications
which tries to keep a measured value at a given target value (the setpoint). The
measured value has to be plugged into the input slot (``input_slot``) and the
output of the PID controller can be read from ``output_slot``.

For example, you can use a PID controller in conjunction with the joints in the
:class:`ODEDynamics` component to keep a hinge or slider at a particular
position. In this case the angle or position is used as input to the PID
controller and the output controls the motor velocity.

.. % \begin{displaymath}
.. % output(t) = K_p \cdot err(t) + K_i \cdot \int err(t')\, dt' + K_d \cdot \frac{d\,err}{dt}
.. % \end{displaymath}


.. class:: PIDController(name = "PIDController",  setpoint = 0.0,  Kp = 0.0,  Ki = 0.0,  Kd = 0.0,  maxout = 999999,  minout = -999999,  auto_insert = True)

   *setpoint* is the target value that should be maintained.

   *Kp* is the weight for the proportional part, *Ki* the weight for the integral
   part and *Kd* the weight for the derivative part.

   *maxout* and *minout* are used to clamp the output value.

A :class:`PIDController` has the following slots:

+-------------------+-------+--------+---------------------------+
| Slot              | Type  | Access | Description               |
+===================+=======+========+===========================+
| ``input_slot``    | float | rw     | The "measured" value      |
+-------------------+-------+--------+---------------------------+
| ``setpoint_slot`` | float | rw     | The target value          |
+-------------------+-------+--------+---------------------------+
| ``output_slot``   | float | r      | Controller output         |
+-------------------+-------+--------+---------------------------+
| ``maxout_slot``   | float | rw     | Maximum output value      |
+-------------------+-------+--------+---------------------------+
| ``minout_slot``   | float | rw     | Minimum output value      |
+-------------------+-------+--------+---------------------------+
| ``Kp_slot``       | float | rw     | Weight for the            |
|                   |       |        | proportional term         |
+-------------------+-------+--------+---------------------------+
| ``Ki_slot``       | float | rw     | Weight for the integral   |
|                   |       |        | term                      |
+-------------------+-------+--------+---------------------------+
| ``Kd_slot``       | float | rw     | Weight for the derivative |
|                   |       |        | term                      |
+-------------------+-------+--------+---------------------------+

