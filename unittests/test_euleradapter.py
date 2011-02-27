# Test the EulerAdapter

import unittest
from cgkit import _core
from cgkit.all import *
from cgkit.sl import radians
    
class TestEulerAdapter(unittest.TestCase):
    
    def testEulerAdapter(self):
        """Check the EulerAdapter component."""

        ea = EulerAdapter(anglex=40, angley=20, anglez=10, order="xyz", auto_insert=False)
        self.assertEqual(ea.output, mat3.fromEulerXYZ(radians(40), radians(20), radians(10)))

        ea.anglex = 0
        ea.angley = 10
        ea.anglez = 20
        self.assertEqual(ea.output, mat3.fromEulerXYZ(0, radians(10), radians(20)))


######################################################################

if __name__=="__main__":
    unittest.main()
