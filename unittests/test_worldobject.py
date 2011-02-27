# Test the WorldObject class

import unittest
from cgkit.all import *
from cgkit._core import _slot_counter


class TestWOConstructor(unittest.TestCase):
    
    def testStandardConstructor(self):
        w = WorldObject()
        self.assertEqual(list(w.iterSlots()), ["angularvel", "cog", "inertiatensor", "linearvel", "mass", "pos", "rot", "scale", "totalmass", "transform", "visible", "worldtransform"])

    def testDefaultNames(self):
        getScene().clear()
        w = WorldObject()
        self.assertEqual(w.name, "object")
        w = WorldObject()
        self.assertEqual(w.name, "object1")
        w = WorldObject()
        self.assertEqual(w.name, "object2")
        
        getScene().clear()
        w = WorldObject()
        self.assertEqual(w.name, "object")

class TestChildHandling(unittest.TestCase):

    def testRenaming(self):
        w = WorldObject(auto_insert=False)
        q = WorldObject(name="spam", auto_insert=False)
        w.addChild(q)
        self.assert_(w.hasChild("spam"))

        q.name = "eggs"
        self.assertEqual(q.name, "eggs")
        self.assert_(w.hasChild("eggs"), "renamed object couldn't be found")
        self.failIf(w.hasChild("spam"))

class TestComparison(unittest.TestCase):

    def testComparison(self):
        w = WorldObject(name="parent", auto_insert=False)
        q = WorldObject(name="child", parent=w)

        self.assertEqual(w,q.parent)


######################################################################

if __name__=="__main__":
    unittest.main()
