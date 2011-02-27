.. % BezierCurveGeom


.. _beziercurvegeom:

:class:`BezierCurveGeom` --- Piecewise cubic Bezier curve
=========================================================

The :class:`BezierCurveGeom` class represents a piecewise cubic curve in 3D
space that is composed of an arbitrary number of cubic Bezier segments. The
class stores a number of 3D points that are interpolated by the curve. Each
point has an in tangent and an out tangent associated with it that define how
the curve enters and leaves the point.


.. class:: BezierPoint(pos, intangent=vec3(0), outtangent=vec3(0))

   This class just stores a position and the in and out tangents and is used to
   pass these parameters to the constructor of a  :class:`BezierCurveGeom`.

   *pos* is a point position that the curve will interpolate.

   *intangent* and *outtangent* define where the curve enters and leaves the point.


.. class:: BezierCurveGeom(pnts = None, closed = False, epsilon = 0.01, subdiv = 4, show_tangents = False)

   *pnts* is a list of :class:`BezierPoint` objects describing the points to
   interpolate and the in and out tangents.

   If *closed* is set to ``True`` the last point will be connected to the first
   point.

   *epsilon* is a threshold value that determines the accuracy of length
   calculations of the curve.

   *subdiv* is the number of subdivisions that are made to draw the curve using
   OpenGL.

   If *show_tangents* is set to ``True`` the OpenGL visualization will also show
   the in and out tangents.

A :class:`BezierCurveGeom` has the following slots:

+----------------------+--------+--------+------------------+
| Slot                 | Type   | Access | Description      |
+======================+========+========+==================+
| ``pnts_slot``        | [vec3] | rw     | The curve points |
+----------------------+--------+--------+------------------+
| ``intangents_slot``  | [vec3] | rw     | The in tangents  |
+----------------------+--------+--------+------------------+
| ``outtangents_slot`` | [vec3] | rw     | The out tangents |
+----------------------+--------+--------+------------------+


.. attribute:: BezierCurveGeom.closed

   This is a boolean indicating wheter the curve is closed or not. You can read and
   write this attribute.


.. attribute:: BezierCurveGeom.numsegs

   The number of Bezier segments in the curve. You can only read this attribute.


.. attribute:: BezierCurveGeom.paraminterval

   This is a tuple (*t_min*, *t_max*) containing the valid parameter interval of
   the curve. You can only read this attribute.

.. % eval


.. method:: BezierCurveGeom.eval(t)

   Evaluate the curve at parameter *t* and return the curve point.

.. % evalFrame


.. method:: BezierCurveGeom.evalFrame(t)

   Evaluate the curve at parameter *t* and return the curve point, the tangent and
   the second derivative.

.. % deriv


.. method:: BezierCurveGeom.deriv(t)

   Return the first derivative (the tangent) at parameter *t*.

.. % arcLen


.. method:: BezierCurveGeom.arcLen(t)

   Return the arc length of the curve up to the point specified by the parameter
   *t*.

.. % length


.. method:: BezierCurveGeom.length()

   Return the entire length of the curve. This is equivalent to  ``arcLen(t_max)``.

