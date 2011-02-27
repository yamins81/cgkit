# Test the Component class

import unittest
from cgkit.all import *
from cgkit._core import _slot_counter


class TestCompConstructor(unittest.TestCase):
    
    def testStandardConstructor(self):
        # Check if a component can be created (without an exception)
        c = Component()
        self.assertEqual(c.numSlots(), 0)
        self.assertEqual(list(c.iterSlots()), [])

    def testConstructorWithName(self):
        c = Component("MyComponent")
        self.assertEqual(c.name, "MyComponent")
        self.assertEqual(c.numSlots(), 0)

class TestCompGetSetName(unittest.TestCase):

    def testGetSetName(self):
        c = Component("MyComponent")
        c.name = "NewName"
        self.assertEqual(c.name, "NewName")

class TestCompHasSlot(unittest.TestCase):

    def testHasSlot1(self):
        c = Component()
        self.assertEqual(c.hasSlot("foo"), 0)

    def testHasSlot2(self):
        c = Component()
        d = DoubleSlot()
        c.addSlot("foo", d)
        self.assertEqual(c.hasSlot("foo"), 1)

class TestCompSlot(unittest.TestCase):

    def testSlot(self):
        c = Component()
        d = DoubleSlot(1.5)
        c.addSlot("foo", d)

        s = c.slot("foo")
        self.assertEqual(s.getValue(), 1.5)

    def testSlotExc(self):
        c = Component()
        d = DoubleSlot(1.5)
        c.addSlot("foo", d)
        self.assertRaises(KeyError, lambda: c.slot("bar"))

class TestCompAddSlot(unittest.TestCase):

    def testAddSlot(self):
        c = Component()
        d = DoubleSlot(1.5)
        c.addSlot("foo", d)
        self.assertEqual(c.hasSlot("foo"), 1)

        self.assertEqual(c.numSlots(), 1)

        s = c.slot("foo")
        self.assertEqual(s.getValue(), 1.5)

    def testAddSlotExc(self):
        c = Component()
        d = DoubleSlot(1.5)
        c.addSlot("foo", d)

        e = DoubleSlot()
        self.assertRaises(KeyError, lambda: c.addSlot("foo", e))

class TestCompRemoveSlot(unittest.TestCase):

    def testRemoveSlot1(self):
        c = Component()
        self.assertEqual(c.numSlots(), 0)
        
        d = DoubleSlot(1.2)
        c.addSlot("foo", d)
        self.assertEqual(c.numSlots(), 1)

        c.removeSlot("foo")
        self.assertEqual(c.numSlots(), 0)

    def testRemoveSlot2(self):
        c = Component()
        self.assertEqual(c.numSlots(), 0)
        
        d = DoubleSlot(1.2)
        c.addSlot("foo", d)
        v = Vec3Slot(vec3(1,2,3))
        c.addSlot("vector", v)
        m = Mat3Slot(mat3(2))
        c.addSlot("matrix", m)

        self.assertEqual(c.numSlots(), 3)
        self.assertEqual(c.slot("foo").getValue(), 1.2)
        self.assertEqual(c.slot("vector").getValue(), vec3(1,2,3))
        self.assertEqual(c.slot("matrix").getValue(), mat3(2))

        c.removeSlot("vector")

        self.assertEqual(c.numSlots(), 2)
        self.assertEqual(c.slot("foo").getValue(), 1.2)
        self.assertEqual(c.slot("matrix").getValue(), mat3(2))

    def testRemoveSlotExc(self):
        c = Component()
        self.assertRaises(KeyError, lambda: c.removeSlot("bar"))

class TestCompSlotIterator(unittest.TestCase):

    def testSlotIterator(self):
        c = Component()
        d = DoubleSlot()
        c.addSlot("double", d)
        v = Vec3Slot(vec3(1,2,3))
        c.addSlot("vector", v)
        m = Mat3Slot(mat3(2))
        c.addSlot("matrix", m)

        names = list(c.iterSlots())
        self.assertEqual(len(names), 3)
        names.sort()
        self.assertEqual(names, ["double", "matrix", "vector"])

class TestCompDeallocation(unittest.TestCase):

    def testDeallocation(self):
        n = _slot_counter()
        c = Component()
        d = DoubleSlot(1.5)
        c.addSlot("foo", d)
        self.assertEqual(_slot_counter(), n+1)

        c.removeSlot("foo")
        self.assertEqual(_slot_counter(), n+1)
        del d
        self.assertEqual(_slot_counter(), n)

######################################################################

if __name__=="__main__":
    unittest.main()
