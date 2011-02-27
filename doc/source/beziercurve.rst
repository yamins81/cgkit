.. % BezierCurve


:class:`BezierCurve` --- Bezier curve
=====================================


.. class:: BezierCurve(name = "BezierCurve",  pnts = None, closed = False, epsilon = 0.01, subdiv = 4, show_tangents = False, curvegeom = None )

   *pnts* is a list of :class:`BezierPoint` objects (see section
   :ref:`beziercurvegeom`).

   If *closed* is set to ``True`` the last point will be connected to the first
   point.

   *epsilon* is a threshold value that determines the accuracy of length
   calculations of the curve.

   *subdiv* is the number of subdivisions that are made to draw the curve using
   OpenGL.

   If *show_tangents* is set to ``True`` the OpenGL visualization will also show
   the in and out tangents.

   You can also pass a previously created :class:`BezierCurveGeom` object via the
   *curvegeom* argument. In this case, the arguments *pnts*, *closed*, *epsilon*,
   *subdiv* and *show_tangents* are  ignored.

