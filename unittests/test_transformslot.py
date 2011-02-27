# Test the transform/pos/rot/scale slots

import unittest
from cgkit.all import *

class TestWithoutControllers(unittest.TestCase):
    
    def testInitialValues(self):
        w = WorldObject()
        self.assertEqual(w.transform, mat4(1))
        self.assertEqual(w.pos, vec3(0))
        self.assertEqual(w.rot, mat3(1))
        self.assertEqual(w.scale, vec3(1))

    def testSetValue_Transform(self):
        M = mat4(1).translate(vec3(1,2,3)).scale(vec3(2,2,2))
        w = WorldObject()
        w.transform = M
        self.assertEqual(w.transform, M)
        self.assertEqual(w.pos, vec3(1,2,3))
        self.assertEqual(w.rot, mat3(1))
        self.assertEqual(w.scale, vec3(2,2,2))

    def testSetValue_Pos(self):
        w = WorldObject()
        w.pos = vec3(2,4,-1)
        self.assertEqual(w.transform, mat4(1).translate(vec3(2,4,-1)))
        self.assertEqual(w.pos, vec3(2,4,-1))
        self.assertEqual(w.rot, mat3(1))
        self.assertEqual(w.scale, vec3(1,1,1))

    def testSetValue_Scale(self):
        w = WorldObject()
        w.scale = vec3(2,4,2)
        self.assertEqual(w.transform, mat4(1).scale(vec3(2,4,2)))
        self.assertEqual(w.pos, vec3(0, 0, 0))
        self.assertEqual(w.rot, mat3(1))
        self.assertEqual(w.scale, vec3(2,4,2))


######################################################################

if __name__=="__main__":
    unittest.main()
