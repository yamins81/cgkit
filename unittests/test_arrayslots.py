# Test the array slots

import unittest
from cgkit import _core
from cgkit.all import *

class TestArraySlot(unittest.TestCase):
    
    def testConstructor(self):
        
        asl = _core.DoubleArraySlot(1)
        # array size==0?
        self.assertEqual(asl.size(), 0)
        # multiplicity==1?
        self.assertEqual(asl.multiplicity(), 1)
        self.assertEqual(list(asl), [])
        self.assertEqual(asl.getController(), None)

        asl = _core.DoubleArraySlot()
        # array size==0?
        self.assertEqual(asl.size(), 0)
        # multiplicity==1?
        self.assertEqual(asl.multiplicity(), 1)
        self.assertEqual(list(asl), [])

        asl = _core.DoubleArraySlot(3)
        # array size==0?
        self.assertEqual(asl.size(), 0)
        # multiplicity==3?
        self.assertEqual(asl.multiplicity(), 3)
        self.assertEqual(list(asl), [])

    def testResize(self):

        asl = _core.DoubleArraySlot(1)

        # Create 8 elements
        asl.resize(8)
        self.assertEqual(asl.size(), 8)
        self.assertEqual(list(asl), 8*[0])

        # Set values
        for i in range(8):
            asl[i]=i+1

        # Check if the values were properly set
        self.assertEqual(list(asl), range(1,9))

        # Enlarge the array
        asl.resize(20)
        self.assertEqual(asl.size(), 20)
        # Check if the values were properly copied
        self.assertEqual(list(asl), range(1,9)+12*[0])

        # Shrink the array
        asl.resize(4)
        # Check if the values were properly copied
        self.assertEqual(list(asl), range(1,5))

        # Negative size must equal 0
        asl.resize(-2)
        self.assertEqual(asl.size(), 0)

    def testSizeConstraints(self):
        """Check if the size constraint really constrains the size.
        """
        
        usc = UserSizeConstraint(4)
        at = _core.DoubleArraySlot(1, usc)
        # Size must be 4
        self.assertEqual(at.size(), 4)
        # resize() must raise an exception
        self.assertRaises(ValueError, lambda: at.resize(5))

        # Resize the constraint
        usc.setSize(7)
        self.assertEqual(at.size(), 7)

        # Check that a slot with a different size cannot serve as controller
        a2 = _core.DoubleArraySlot(1)
        a2.resize(10)
        self.assertRaises(ValueError, lambda: at.setController(a2))

        # But it can be controller when it has the same size...
        a2.resize(7)
        at.setController(a2)
        # Check that the controller cannot be resized then...
        self.assertRaises(ValueError, lambda: a2.resize(10))
        self.assertEqual(usc.getSize(), 7)
        self.assertEqual(at.size(), 7)

        # Check the isResizable() method
        self.assertEqual(True, a2.isResizable(7))
        self.assertEqual(True, at.isResizable(7))
        self.assertEqual(False, a2.isResizable(42))
        self.assertEqual(False, at.isResizable(42))

    def testLinearSizeConstraints(self):
        """Test the LinearSizeConstraint class.
        """

        a1 = _core.IntArraySlot(1)
        a1.resize(4)
        
        lsc = LinearSizeConstraint(a1, 2, 1)
        self.assertEqual(lsc.getSize(), 9)

        # Create a slot that depends on the size of a1
        a2 = _core.IntArraySlot(1, lsc)    
        self.assertEqual(a2.size(), 9)

        a1.resize(6)
        self.assertEqual(a2.size(), 13)

    def testMultipleSizeConstraints(self):
        """Test several constraints that influence each other.
        """

        a1 = _core.IntArraySlot(1)
        a1.resize(4)

        lsc = LinearSizeConstraint(a1,1,1)
        a2 = _core.IntArraySlot(1, lsc)

        usc = UserSizeConstraint(5)
        a3 = _core.IntArraySlot(1, usc)

        a4 = _core.IntArraySlot(1)
        
        self.assertEqual(0, a3[1])

        a2.connect(a4)
        a2.connect(a3)
        self.assertEqual(5, a4.size())
        a2[1] = 12;
        self.assertEqual(12, a2[1])
        self.assertEqual(12, a3[1])
        self.assertEqual(12, a4[1])

        # Resizing the first slot must fail because of the other, size
        # constraint slots...
        self.assertRaises(ValueError, lambda: a1.resize(2))

        # Check that the slots still have the original size...
        self.assertEqual(4, a1.size())
        self.assertEqual(5, a2.size())
        self.assertEqual(5, a3.size())
        self.assertEqual(5, a4.size())

    def testGetSetMult1(self):

        # Multiplicity 1
        asl = _core.Vec3ArraySlot(1)
        asl.resize(10)

        for i in range(10):
            asl.setValue(i, vec3(i+1))

        for i in range(10):
            self.assertEqual(asl.getValue(i), vec3(i+1))

        for i in range(10):
            self.assertEqual(asl[i], vec3(i+1))

    def testGetSetMultn(self):

        # Multiplicity 3
        asl = _core.IntArraySlot(3)
        asl.resize(10)

        for i in range(10):
            asl.setValue(i, (i, i+1, i+2))

        for i in range(10):
            self.assertEqual(asl.getValue(i), (i,i+1,i+2))

        for i in range(10):
            self.assertEqual(asl[i], (i,i+1,i+2))

    def testController(self):

        # Controller slot
        ctrl = _core.Vec3ArraySlot()
        ctrl.resize(10)
        for i in range(10):
            ctrl[i] = vec3(i+1)

        # "Target" slot
        asl = _core.Vec3ArraySlot()
        asl.setController(ctrl)
        self.assertEqual(asl.size(), 10)
        for i in range(10):
            self.assertEqual(asl[i], vec3(i+1))

        # Change an item in the controller...
        ctrl[1]=vec3(-1,-2,-3)
        # ...and see if the target also has changed
        for i in range(10):
            if i==1:
                self.assertEqual(asl[i], vec3(-1,-2,-3))
            else:
                self.assertEqual(asl[i], vec3(i+1))

        # Disconnect
        asl.setController(None)
        # modify an element
        asl[1] = vec3(2,2,2)
        # and check if ctrl and target are separate
        for i in range(10):
            if i==1:
                self.assertEqual(ctrl[i], vec3(-1,-2,-3))
            else:
                self.assertEqual(ctrl[i], vec3(i+1))
        
        for i in range(10):
            self.assertEqual(asl[i], vec3(i+1))
        

    def testExceptions(self):
        
        asl = _core.DoubleArraySlot(1)
        asl.resize(8)
        self.assertRaises(IndexError, lambda: asl[8])
        self.assertRaises(IndexError, lambda: asl.getValue(8))
        self.assertRaises(IndexError, lambda: asl.setValue(8,0))
        self.assertRaises(TypeError, lambda: asl.setValue(0,vec3()))

        asl = _core.DoubleArraySlot(2)
        asl.resize(8)
        self.assertRaises(IndexError, lambda: asl[8])
        self.assertRaises(IndexError, lambda: asl.getValue(8))
        self.assertRaises(IndexError, lambda: asl.setValue(8,(1,2)))
        self.assertRaises(ValueError, lambda: asl.setValue(0,5))


######################################################################

if __name__=="__main__":
    unittest.main()
