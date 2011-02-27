.. % FlockOfBirds component


:class:`FlockOfBirds` --- Retrieving values from an Ascension Flock of Birds motion tracker
===========================================================================================

The :class:`FlockOfBirds` class communicates with an Ascension Flock of Birds
motion tracker  (`<http://www.ascension-tech.com/>`_) via the serial port and
provides the  sensor values of the individual birds via slots.

Note: Currently, the class assumes that an extended range controller is used!
(this means the position values have to be adjusted if you use the tracker
without an extended range controller)


.. class:: FlockOfBirds(name = "FlockOfBirds",  com_port = 0, baud_rate = 115200, timeout = 2.0, num_birds = 1, bird_mode = "p", hemisphere = "forward", auto_insert = True)

   *com_port* specifies which COM port to use for the communication with the flock
   (0-based port number).

   *baud_rate* is the baud rate to use.

   *timeout* specifies a time span in seconds after which a RS232 operation fails.

   *num_birds* is the number of birds to use (including the ERC which will  be bird
   1).

   *bird_mode* selects what kind of data the birds will send. It can be either a
   string containing the mode for all birds or it can be a list of strings to
   select a mode for each individual bird. In the latter case, the list must hold
   one string for each bird. The mode string can be one of the values in the
   following table:

   +----------+---------------------------------+
   | Mode     | Description                     |
   +==========+=================================+
   | ``"p"``  | Send only the position.         |
   +----------+---------------------------------+
   | ``"a"``  | Send only euler angles.         |
   +----------+---------------------------------+
   | ``"m"``  | Send only a rotation matrix.    |
   +----------+---------------------------------+
   | ``"q"``  | Send only a quaternion.         |
   +----------+---------------------------------+
   | ``"pa"`` | Send position and euler angles. |
   +----------+---------------------------------+
   | ``"pm"`` | Send position and matrix.       |
   +----------+---------------------------------+
   | ``"pq"`` | Send position and quaternion.   |
   +----------+---------------------------------+

   *hemisphere* specifies in which hemisphere, centered about the transmitter, the
   sensor will be operating (see the Flock of Birds --- Installation and Operation
   Guide). The values can be one of ``"forward"``, ``"aft"``, ``"upper"``,
   ``"lower"``, ``"left"`` and ``"right"``.

For each bird, the :class:`FlockOfBirds` component creates the following four
slots:

* ``pos<n>_slot`` (``Vec3Slot``) -- Position (in cm)

* ``angle<n>_slot`` (``Vec3Slot``) -- Euler angles

* ``matrix<n>_slot`` (``Mat3Slot``) -- Rotation matrix

* ``quat<n>_slot`` (``QuatSlot``) -- Quaternion

where ``<n>`` is the number of the bird (1-based). Not all of the slots are
active at the same time. You can select which slot should carry the
corresponding value via the bird mode.

.. note::

   To use the :class:`FlockOfBirds` component the `pySerial
   <http://pyserial.sourceforge.net/>`_ module has to be  installed on your system.

