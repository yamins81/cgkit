# Test the PolyhedronGeom

import unittest
from cgkit import _core
from cgkit.all import *

class TestPolyhedron(unittest.TestCase):
    
    def testVariables_Size(self):

#        _core._set_debug_flag(True)
        pg = PolyhedronGeom()
        pg.verts.resize(8)
        pg.setNumPolys(4)
        for i in range(4):
            pg.setPoly(i, [[0,1,2,3]])

        # Create new variables and check the sizes of the slots...
        pg.newVariable("constant", CONSTANT, INT)
        c_slot = pg.slot("constant")
        self.assertEqual(c_slot.size(), 1)

        pg.newVariable("uniform", UNIFORM, INT)
        u_slot = pg.slot("uniform")
        self.assertEqual(u_slot.size(), 4)

        pg.newVariable("varying", VARYING, INT)
        v_slot = pg.slot("varying")
        self.assertEqual(v_slot.size(), 8)

        pg.newVariable("vertex", VERTEX, INT)
        vx_slot = pg.slot("vertex")
        self.assertEqual(vx_slot.size(), 8)

        pg.newVariable("facevarying", FACEVARYING, INT)
        fv_slot = pg.slot("facevarying")
        self.assertEqual(fv_slot.size(), 16)

        pg.newVariable("facevertex", FACEVERTEX, INT)
        fvx_slot = pg.slot("facevertex")
        self.assertEqual(fvx_slot.size(), 16)

        # Resize the mesh...
        pg.verts.resize(6)
        pg.setNumPolys(3)

        # ...and check the variables again
        self.assertEqual(c_slot.size(), 1)
        self.assertEqual(u_slot.size(), 3)
        self.assertEqual(v_slot.size(), 6)
        self.assertEqual(vx_slot.size(), 6)
        self.assertEqual(fv_slot.size(), 12)
        self.assertEqual(fvx_slot.size(), 12)

        # Change one poly...
        pg.setPoly(1, [[1,2,3,4,5], [0,3,4,6]])
        # ...and check the variables again
        self.assertEqual(c_slot.size(), 1)
        self.assertEqual(u_slot.size(), 3)
        self.assertEqual(v_slot.size(), 6)
        self.assertEqual(vx_slot.size(), 6)
        self.assertEqual(fv_slot.size(), 17)
        self.assertEqual(fvx_slot.size(), 17)
        
        # and another round...
        pg.setPoly(1, [[1,2,3]])
        # ...and check the variables again
        self.assertEqual(c_slot.size(), 1)
        self.assertEqual(u_slot.size(), 3)
        self.assertEqual(v_slot.size(), 6)
        self.assertEqual(vx_slot.size(), 6)
        self.assertEqual(fv_slot.size(), 11)
        self.assertEqual(fvx_slot.size(), 11)
        

######################################################################

if __name__=="__main__":
    unittest.main()
