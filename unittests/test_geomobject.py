# Test the Component class

import unittest
from cgkit.all import *
import cgkit._core

class MyGeom(cgkit._core.GeomObject):

    def __init__(self):
        cgkit._core.GeomObject.__init__(self)

    def slotSizeConstraint(self, storage):
        if storage==UNIFORM:
            return UserSizeConstraint(1)
        elif storage==VARYING or storage==VERTEX:
            return UserSizeConstraint(2)
        elif storage==FACEVARYING or storage==FACEVERTEX:
            return UserSizeConstraint(3)
        else:
            return None

    def boundingBox(self):
        bb = BoundingBox()
        bb.addPoint(vec3(-1,-2,-3))
        bb.addPoint(vec3(2,3,4))
        return bb

    def drawGL(self):
        pass

    def convert(self, geom):
        pass

######################################################################

class TestDerivedPyGeoms(unittest.TestCase):

    def testPyDerived(self):
        mg = MyGeom()

        self.assertEqual(mg.uniformCount(), 1)
        self.assertEqual(mg.varyingCount(), 2)
        self.assertEqual(mg.vertexCount(), 2)
        self.assertEqual(mg.faceVaryingCount(), 3)
        self.assertEqual(mg.faceVertexCount(), 3)

        mg.newVariable("foo_uniform", UNIFORM, INT)
        s = mg.slot("foo_uniform")
        self.assertEqual(s.size(), 1)

        mg.newVariable("foo_varying", VARYING, INT)
        s = mg.slot("foo_varying")
        self.assertEqual(s.size(), 2)

        mg.newVariable("foo_vertex", VERTEX, INT)
        s = mg.slot("foo_vertex")
        self.assertEqual(s.size(), 2)

        mg.newVariable("foo_facevarying", FACEVARYING, INT)
        s = mg.slot("foo_facevarying")
        self.assertEqual(s.size(), 3)

        w=WorldObject("dummy")
        w.geom = mg
        b1,b2 = getScene().boundingBox().getBounds()
        self.assertEqual(b1, vec3(-1,-2,-3))
        self.assertEqual(b2, vec3(2,3,4))

        tm = TriMeshGeom()
        mg.convert(tm)

######################################################################

if __name__=="__main__":
    unittest.main()


