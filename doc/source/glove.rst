
:mod:`glove` --- Wrapper around the 5DT Data Glove SDK
======================================================

.. module:: cgkit.glove
   :synopsis: Wrapper around the 5DT Data Glove SDK


This module contains a wrapper around the Data Glove SDK by `Fifth Dimension
Technologies <http://www.5dt.com/>`_. The underlying SDK can either be version 1
or version 2. Note that some methods are only implemented if the module was
compiled with version 2 of the SDK. All the functions from the SDK are wrapped
as methods of the  :class:`DataGlove` class. Most methods require that a
connection to a glove device was previously established via the :meth:`open`
method.

.. function:: available()

   Returns ``True`` if the Data Glove functionality is available.

   If this function returns ``False``, an exception will be raised whenever you try
   to instantiate a class from this module.

.. class:: DataGlove()

   This class encapsulates the data glove handle and contains the functions from
   the SDK as methods. Before you can use most of the methods, you have to
   establish a connection to a data glove by calling the :meth:`open` method.

.. method:: DataGlove.isConnected()

   Returns ``True`` if a connection to a data glove was established (i.e. if
   :meth:`open` was successfully called).

.. method:: DataGlove.open(port)

   Establish a connection to the data glove at the specified port. *port* is a
   string containing the serial port to use. For example, under Windows you might
   specify ``"COM1"``, ``"COM2"``, etc.

.. method:: DataGlove.close()

   Disconnect from the glove. The return value is a boolean indicating if there was
   an error. Calling :meth:`close` while there is no connection established is not
   considered an error.

.. method:: DataGlove.getGloveHand()

   Return the handedness of the glove. The return value is either ``FD_HAND_LEFT``
   or ``FD_HAND_RIGHT``. If you want a string representation of the handedness you
   can use the return value as key for the dictionary ``HANDEDNESS``.

.. method:: DataGlove.getGloveType()

   Return the type of the data glove. The return value is one of the following:

   +---------------------+--------------------------------------+
   | Type                | Description                          |
   +=====================+======================================+
   | ``FD_GLOVENONE``    | No data glove                        |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE5U``      | Data Glove 5 Ultra serial            |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE5UW``     | Data Glove 5 Ultra serial, wireless  |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE5U_USB``  | Data Glove 5 Ultra USB               |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE7``       | Data Glove 5                         |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE7W``      | Data Glove 5, wireless               |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE16``      | Data Glove 16                        |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE16W``     | Data Glove 16, wireless              |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE14U``     | Data Glove 14 Ultra serial           |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE14UW``    | Data Glove 14 Ultra serial, wireless |
   +---------------------+--------------------------------------+
   | ``FD_GLOVE14U_USB`` | Data Glove 14 Ultra USB              |
   +---------------------+--------------------------------------+

   If you want the string representation of the glove type you can use the return
   value as key for the dictionary ``GLOVETYPE``.

   Todo: The above are only the return values of the v2 SDK.

.. % getNumSensors


.. method:: DataGlove.getNumSensors()

   Return the number of sensors.

.. % getNumGestures


.. method:: DataGlove.getNumGestures()

   Return the number of available gestures.

.. % getSensorRawAll


.. method:: DataGlove.getSensorRawAll()

   Return a list with the raw sensor values of all sensors.

.. % getSensorRaw


.. method:: DataGlove.getSensorRaw(sensor)

   Return the raw sensor value of the specified sensor. *sensor* is an integer in
   the range between 0 and :meth:`getNumSensors`\ -1.

.. % setSensorRawAll


.. method:: DataGlove.setSensorRawAll(data)


.. method:: DataGlove.setSensorRaw(sensor, raw)

.. % getSensorScaledAll


.. method:: DataGlove.getSensorScaledAll()

   Return a list of the scaled sensor values of all sensors.

.. % getSensorScaled


.. method:: DataGlove.getSensorScaled(sensor)

   Return the scaled sensor value of the specified sensor. *sensor* is an integer
   in the range between 0 and :meth:`getNumSensors`\ -1.

.. % getCalibrationAll


.. method:: DataGlove.getCalibrationAll()

   Return the current auto-calibration settings for all sensors. The return value
   is a 2-tuple (*lower*, *upper*) where *lower* is a list with the minimum values
   and *upper* a list with maximum values.

.. % getCalibration


.. method:: DataGlove.getCalibration(sensor)

   Return the auto-calibration for one particular sensor. The return value is a
   2-tuple (*lower*, *upper*) that contains the minimum and maximum value.

.. % setCalibrationAll


.. method:: DataGlove.setCalibrationAll(lower, upper)

   Set the auto-calibration settings. *lower* is a list with the minimum sensor
   values and *upper* is a list with the maximum values.

.. % setCalibration


.. method:: DataGlove.setCalibration(sensor, lower, upper)

   Set the auto-calibration values for one sensor. *lower* is the minimum sensor
   value and *upper* is the maximum value.

.. % getSensorMaxAll


.. method:: DataGlove.getSensorMaxAll()

   Return a list with the maximum scaled values of the sensors.

.. % getSensorMax


.. method:: DataGlove.getSensorMax(sensor)

   Return a maximum scaled sensor value for one particular sensor.

.. % setSensorMaxAll


.. method:: DataGlove.setSensorMaxAll(max)

   Set the maximum scaled sensor values. *max* is a list with the values.

.. % setSensorMax


.. method:: DataGlove.setSensorMax(sensor, max)

   Set the maximum scaled sensor value for one sensor.

.. % resetCalibration


.. method:: DataGlove.resetCalibration([sensor])

   Reset the auto-calibration values. In version 2 of the SDK you can pass an
   optional sensor index which will only reset that particular sensor.

.. % getGesture


.. method:: DataGlove.getGesture()

   Return the current gesture.

.. % getThresholdAll


.. method:: DataGlove.getThresholdAll()

   Return the current gesture recognition threshold values. The return value is a
   2-tuple (*lower*, *upper*) that contains the lower and upper threshold values.

.. % getThreshold


.. method:: DataGlove.getThreshold(sensor)

   Return the gesture recognition threshold values for one sensor. The return value
   is a 2-tuple (*lower*, *upper*) that contains the lower and upper threshold
   value.

.. % setThresholdAll


.. method:: DataGlove.setThresholdAll(lower, upper)

   Set the gesture recognition threshold values for all sensors. *lower* is a list
   with the lower threshold values and *upper* is a list with the upper threshold
   values.

.. % setThreshold


.. method:: DataGlove.setThreshold(sensor, lower, upper)

   Set the gesture recognition threshold values for one sensor. *lower* is the
   lower threshold value and *upper* is the upper threshold value.

.. % getGloveInfo


.. method:: DataGlove.getGloveInfo()

   Return a string with glove information.

.. % getDriverInfo


.. method:: DataGlove.getDriverInfo()

   Return a string with driver information.

.. % scanUSB


.. method:: DataGlove.scanUSB()

   Scans the USB for available gloves. The return value is a list with product IDs
   of the gloves found. The product IDs can be one of ``DG14U_R``,  ``DG14U_L``,
   ``DG5U_R`` and ``DG5U_R``. ---  Availability: V2

.. % setCallback


.. method:: DataGlove.setCallback(callback)

   Set a callback function that gets called when a new packet was received.
   *callback* must be a callable object. ---  Availability: V2

.. % getPacketRate


.. method:: DataGlove.getPacketRate()

   Return the current packet rate in Hertz. ---  Availability: V2

.. % newData


.. method:: DataGlove.newData()

   Returns a boolean that indicates whether new data is available or not. ---
   Availability: V2

.. % getFWVersionMajor


.. method:: DataGlove.getFWVersionMajor()

   Return the major version of the glove's firmware. This is only implemented for
   the Data Glove 14 Ultra. Other gloves will always return 0. ---  Availability:
   V2

.. % getFWVersionMinor


.. method:: DataGlove.getFWVersionMinor()

   Return the minor version of the glove's firmware. This is only implemented for
   the Data Glove 14 Ultra. Other gloves will always return 0. ---  Availability:
   V2

.. % getAutoCalibrate


.. method:: DataGlove.getAutoCalibrate()

   Returns ``True`` if auto calibration is activated. ---  Availability: V2

.. % setAutoCalibrate


.. method:: DataGlove.setAutoCalibrate(flag)

   Enable or disable auto-calibration. If *flag* is ``True``  auto-calibration is
   enabled. ---  Availability: V2

.. % saveCalibration


.. method:: DataGlove.saveCalibration(filename)

   Save the current calibration data to disk. ---  Availability: V2

.. % loadCalibration


.. method:: DataGlove.loadCalibration(filename)

   Load the calibration settings from a file. ---  Availability: V2

.. note::

   The module uses the SDK by Fifth Dimension Technologies (5DT) which can be found
   at `<http://www.5dt.com/>`_. The following is the copyright information of the
   SDK:

   *Copyright (C) 2000-2004, 5DT <Fifth Dimension Technologies>*

