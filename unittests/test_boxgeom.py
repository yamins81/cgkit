# Test the BoxGeom

import unittest
from cgkit import _core
from cgkit.all import *
from _utils import *    
    

class TestBoxGeom(unittest.TestCase):
    
    def testVariables_Size(self):
        """Check the size of primitive variables."""

        geom = BoxGeom()

        self.assertEqual(geom.uniformCount(), 6)
        self.assertEqual(geom.varyingCount(), 8)
        self.assertEqual(geom.vertexCount(), 8)
        self.assertEqual(geom.faceVaryingCount(), 0)
        self.assertEqual(geom.faceVertexCount(), 0)

        # Create new variables and check the sizes of the slots...
        createVars(geom)
        checkVars(self, geom, uniform=6, var=8, vtx=8, facevar=0, facevtx=0)

        # Check that slots can't be resized (except user vars)
        checkVarResize(self, geom)

######################################################################

if __name__=="__main__":
    unittest.main()
