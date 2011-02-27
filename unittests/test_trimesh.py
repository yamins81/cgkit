# Test the TriMeshGeom

import unittest
from cgkit import _core
from cgkit.all import *
from _utils import *    
    

class TestTriMesh(unittest.TestCase):
    
    def testVariables_Size(self):

        tm = TriMeshGeom()
        tm.verts.resize(8)
        tm.faces.resize(4)

        self.assertEqual(tm.uniformCount(), 4)
        self.assertEqual(tm.varyingCount(), 8)
        self.assertEqual(tm.vertexCount(), 8)
        self.assertEqual(tm.faceVaryingCount(), 12)
        self.assertEqual(tm.faceVertexCount(), 12)

        # Create new variables and check the sizes of the slots...
        createVars(tm)
        checkVars(self, tm, uniform=4, var=8, vtx=8, facevar=12, facevtx=12)

        # Resize the mesh...
        tm.verts.resize(6)
        tm.faces.resize(5)

        # ...and check the variables again
        checkVars(self, tm, uniform=5, var=6, vtx=6, facevar=15, facevtx=15)

        # Check that slots can't be resized (except user vars)
        checkVarResize(self, tm)

######################################################################

if __name__=="__main__":
    unittest.main()
