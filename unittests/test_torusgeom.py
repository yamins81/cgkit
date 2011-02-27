# Test the TorusGeom

import unittest
from cgkit import _core
from cgkit.all import *
from _utils import *    
    

class TestTorusGeom(unittest.TestCase):
    
    def testVariables_Size(self):
        """Check the size of primitive variables."""

        geom = TorusGeom()

        self.assertEqual(geom.uniformCount(), 1)
        self.assertEqual(geom.varyingCount(), 4)
        self.assertEqual(geom.vertexCount(), 4)
        self.assertEqual(geom.faceVaryingCount(), 0)
        self.assertEqual(geom.faceVertexCount(), 0)

        # Create new variables and check the sizes of the slots...
        createVars(geom)
        checkVars(self, geom, uniform=1, var=4, vtx=4, facevar=0, facevtx=0)

        # Check that slots can't be resized (except user vars)
        checkVarResize(self, geom)

######################################################################

if __name__=="__main__":
    unittest.main()
