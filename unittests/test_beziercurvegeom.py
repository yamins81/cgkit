# Test the BezierCurveGeom class

import unittest
from cgkit.beziercurvegeom import *
from math import *
from _utils import *    


class TestBezierCurveGeom(unittest.TestCase):

    def testParamInterval(self):
        """Test the parameter interval."""

        # Open curves
        pnts = []
        for i in range(10):
            pnts.append(BezierPoint((i,0,0)))
            crv = BezierCurveGeom(pnts=pnts, closed=False)
            self.assertEqual(crv.paraminterval, (0,i))

        # Closed curves
        pnts = []
        for i in range(10):
            pnts.append(BezierPoint((i,0,0)))
            crv = BezierCurveGeom(pnts=pnts, closed=True)
            self.assertEqual(crv.paraminterval, (0,i+1))

        # Close an open curve
        crv = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                      BezierPoint((2,0,0)),
                                      BezierPoint((2,1,0)),
                                      BezierPoint((1,1,0))],
                              closed = False)
        self.assertEqual(crv.paraminterval, (0,3))
        crv.closed = True
        self.assertEqual(crv.paraminterval, (0,4))
        # ...and open it again
        crv.closed = False
        self.assertEqual(crv.paraminterval, (0,3))

    def testCurveLength(self):
        """Test the length of a curve."""

        crv = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                      BezierPoint((2,0,0)),
                                      BezierPoint((2,1,0)),
                                      BezierPoint((1,1,0))],
                              closed = False)
        self.assertEqual(crv.length(), 3.0)
        self.assertEqual(crv.arcLen(1.0), 1.0)
        self.assertEqual(crv.arcLen(2.0), 2.0)
        self.assertEqual(crv.arcLen(3.0), 3.0)
        crv.closed = True
        self.assertEqual(crv.length(), 4.0)

        # Create the same curve as above but set one tangent != 0
        # -> The 1st segment is not straight anymore and the length
        # must have increased
        crv2 = BezierCurveGeom(pnts = [BezierPoint((1,0,0), outtangent=(0.5,0.5,0)),
                                       BezierPoint((2,0,0)),
                                       BezierPoint((2,1,0)),
                                       BezierPoint((1,1,0))],
                              closed = True)
        self.failUnless(crv2.length()>crv.length(),
                        "Curve length didn't increase: %f "%crv2.length())

    def testCurveLengthChange(self):
        """Test if the length of a curve changes when the curve is modified."""

        crv = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                      BezierPoint((2,0,0)),
                                      BezierPoint((2,1,0)),
                                      BezierPoint((1,1,0))],
                              closed = True)
        self.assertEqual(crv.length(), 4.0)

        crv.pnts[2] = vec3(3,1,0)
        self.assertEqual(crv.length(), 4.0+sqrt(2))

        crv.pnts.resize(5)
        self.assertEqual(crv.length(), 4.0+2*sqrt(2))

    def testEval(self):
        """Evaluate the curve."""

        crv = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                      BezierPoint((2,0,0)),
                                      BezierPoint((2,1,0)),
                                      BezierPoint((1,1,0))],
                              closed = False)

        # Check start point
        self.assertEqual(crv.eval(0.0), vec3(1,0,0))
        # Check end point
        self.assertEqual(crv.eval(3.0), vec3(1,1,0))

        # Middle control points
        self.assertEqual(crv.eval(1.0), vec3(2,0,0))
        self.assertEqual(crv.eval(2.0), vec3(2,1,0))

        # Inbetween points
        self.assertEqual(crv.eval(0.5), vec3(1.5,0,0))
        self.assertEqual(crv.eval(1.5), vec3(2,0.5,0))
        self.assertEqual(crv.eval(2.5), vec3(1.5,1,0))

    def testDeriv(self):
        """Check the 1st derivative."""

        crv = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                      BezierPoint((2,0,0)),
                                      BezierPoint((2,1,0)),
                                      BezierPoint((1,1,0))],
                              closed = False)

        # Check start point
        self.assertEqual(crv.deriv(0.0), vec3(0,0,0))
        # Check end point
        self.assertEqual(crv.deriv(3.0), vec3(0,0,0))

        # Inbetween points
        self.assertEqual(crv.deriv(0.5).normalize(), vec3(1,0,0))
        self.assertEqual(crv.deriv(1.5).normalize(), vec3(0,1,0))
        self.assertEqual(crv.deriv(2.5).normalize(), vec3(-1,0,0))

    def testVariables_Size(self):
        """Check the size of primitive variables."""

        geom = BezierCurveGeom(pnts = [BezierPoint((1,0,0)),
                                       BezierPoint((2,0,0)),
                                       BezierPoint((2,1,0)),
                                       BezierPoint((1,1,0))],
                               closed = False)

        self.assertEqual(geom.uniformCount(), 1)
        self.assertEqual(geom.varyingCount(), 4)
        self.assertEqual(geom.vertexCount(), 10)
        self.assertEqual(geom.faceVaryingCount(), 0)
        self.assertEqual(geom.faceVertexCount(), 0)

        # Create new variables and check the sizes of the slots...
        createVars(geom)
        checkVars(self, geom, uniform=1, var=4, vtx=10, facevar=0, facevtx=0)

        # Close the curve and check the sizes again...
        geom.closed = True
        checkVars(self, geom, uniform=1, var=4, vtx=12, facevar=0, facevtx=0)
        geom.closed = False
        checkVars(self, geom, uniform=1, var=4, vtx=10, facevar=0, facevtx=0)

        # Add a point...
        geom.pnts.resize(5)
        checkVars(self, geom, uniform=1, var=5, vtx=13, facevar=0, facevtx=0)

        # Check that slots can't be resized (except user vars)
        checkVarResize(self, geom)


######################################################################

if __name__=="__main__":
    unittest.main()
