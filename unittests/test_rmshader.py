# Test the RMShader class

import unittest
from cgkit.rmshader import RMShader
from cgkit.cgtypes import *

class TestRMShader(unittest.TestCase):
    """Test the RMShader class.
    """
    
    def testShader(self):
        """Test the RMShader class.
        """
        shd = RMShader("data/testshader.sl")
        # Attribute access
        self.assertEqual(1.0, shd.Ka)
        self.assertEqual((0,0,0), shd.norm)
        self.assertEqual([1.0, 2.0], shd.uv)
        self.assertEqual((0,0,0), shd.out)
        self.assertEqual([(1.0,2.0,3.0), (4.0,5.0,6.0), (7.0,8.0,9.0)], shd.pnts)
        self.assertEqual((1.0,1.0,1.0), shd.col)
        self.assertEqual(mat4(1), shd.M)
        self.assertEqual(0.5, shd.Kd)
        self.assertEqual([0.5], shd.singleArr)

        # Slot access
        self.assertEqual(1.0, shd.Ka_slot.getValue())
        self.assertEqual((0,0,0), shd.norm_slot.getValue())
        self.assertEqual(1.0, shd.uv_slot.getValue(0))
        self.assertEqual(2.0, shd.uv_slot.getValue(1))
        self.assertEqual((0,0,0), shd.out_slot.getValue())
        self.assertEqual((1.0,2.0,3.0), shd.pnts_slot.getValue(0))
        self.assertEqual((4.0,5.0,6.0), shd.pnts_slot.getValue(1))
        self.assertEqual((7.0,8.0,9.0), shd.pnts_slot.getValue(2))
        self.assertEqual((1.0,1.0,1.0), shd.col_slot.getValue())
        self.assertEqual(mat4(1), shd.M_slot.getValue())
        self.assertEqual(0.5, shd.Kd_slot.getValue())
        self.assertEqual(0.5, shd.singleArr_slot.getValue(0))

        # Assign some new values
        shd.Ka = 0.7
        self.assertEqual(0.7, shd.Ka_slot.getValue())
        shd.singleArr = [0.6]
        self.assertEqual(0.6, shd.singleArr_slot.getValue(0))
        
    def testConstructor(self):
        """Test passing initial parameter values in the constructor.
        """
        shd = RMShader("data/testshader.sl", Ka=0.7, norm=(0,0,1), uv=(0.2,0.3), singleArr=[0.8])
        self.assertEqual(0.7, shd.Ka_slot.getValue())
        self.assertEqual((0,0,1), shd.norm_slot.getValue())
        self.assertEqual(0.2, shd.uv_slot.getValue(0))
        self.assertEqual(0.3, shd.uv_slot.getValue(1))
        self.assertEqual(0.8, shd.singleArr_slot.getValue(0))
        

######################################################################

if __name__=="__main__":
    unittest.main()
