# Test the bounding box class

import unittest
from cgkit.boundingbox import BoundingBox

# Tests vervollstaendigen! Exception, falls getBounds() mit leerer Box
# aufgerufen wird.

class TestBBConstructor(unittest.TestCase):
    
    def testStandardConstructor(self):
        bb = BoundingBox()
        self.failUnless(bb.isEmpty(), "Newly constructed bbox is not empty!")
#        bmin, bmax = bb.getBounds()
#        print bmin, bmax

######################################################################

if __name__=="__main__":
    unittest.main()
