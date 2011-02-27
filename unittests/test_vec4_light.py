# Test the vec4 class

import unittest
#from cgkit.cgtypes import *
from cgkit.light.cgtypes import *
import math, os, pickle

class TestVec4(unittest.TestCase):


    ######################################################################
    def testConstructor(self):
        v = vec4(1.5,-2,3,2)
        self.failUnless(v.x==1.5 and v.y==-2.0 and v.z==3 and v.w==2,
                        "vec4(1.5,-2,3,2) != %s"%str(v))

        v = vec4(1.5,-2,3)
        self.failUnless(v.x==1.5 and v.y==-2.0 and v.z==3 and v.w==0,
                        "vec4(1.5,-2,3) != vec4(1.5,-2,3,0) : %s"%str(v))

        v = vec4()
        self.failUnless(v==vec4(0,0,0,0),"vec4() != (0,0,0,0) : %s"%str(v))

        v = vec4(2.5)
        self.failUnless(v==vec4(2.5,2.5,2.5,2.5),
                        "vec4(2.5) != (2.5,2.5,2.5,2.5) : %s"%str(v))

        v = vec4(-1.5, 42)
        self.failUnless(v==vec4(-1.5, 42.0, 0.0),
                        "vec4(-1.5, 42) != (-1.5, 42,0,0) : %s"%str(v))

        v = vec4(())
        self.failUnless(v==vec4(0, 0, 0, 0),
                        "vec4(()) != (0,0,0,0) : %s"%str(v))

        v = vec4([1])
        self.failUnless(v==vec4(1, 1, 1, 1),
                        "vec4([1]) != (1,1,1,1) : %s"%str(v))

        v = vec4((1,2))
        self.failUnless(v==vec4(1, 2, 0, 0),
                        "vec4((1,2)) != (1,2,0,0) : %s"%str(v))

        v = vec4((1,2,3))
        self.failUnless(v==vec4(1, 2, 3, 0),
                        "vec4((1,2,3)) != (1,2,3,0) : %s"%str(v))

        v = vec4((1,2,3,4))
        self.failUnless(v==vec4(1, 2, 3, 4),
                        "vec4((1,2,3,4)) != (1,2,3,4) : %s"%str(v))

        w = vec4(7,-2,3,2)
        v = vec4(w)
        self.failUnless(v==vec4(7, -2, 3, 2),
                        "vec4(vec4(7,-2,3,2)) != (7,-2,3,2) : %s"%str(v))
        w.x = 12
        self.failUnless(v==vec4(7, -2, 3, 2),
                        "vec4(vec4) doesn't create a copy : %s"%str(v))

        v = vec4("1,2,3")
        self.failUnless(v==vec4(1, 2, 3, 0),
                        'vec4("1,2,3") != (1,2,3,0) : %s'%str(v))

        v = vec4("1,2")
        self.failUnless(v==vec4(1, 2, 0, 0),
                        'vec4("1,2") != (1,2,0,0) : %s'%str(v))

        v = vec4("1")
        self.failUnless(v==vec4(1, 1, 1, 1),
                        'vec4("1") != (1,1,1,1) : %s'%str(v))

        v = vec4("")
        self.failUnless(v==vec4(0, 0, 0),
                        'vec4("") != (0,0,0,0) : %s'%str(v))

        try:
            v = vec4("x,y,z,w")
            self.fail('vec4("x,y,z,w") doesn\'t throw an exception')
        except ValueError:
            pass

        try:
            v = vec4(1,2,3,4,5)
            self.fail("vec4(1,2,3,4,5) doesn't throw an exception")
        except TypeError:
            pass

        try:
            v = vec4((1,2,3,4,5))
            self.fail("vec4((1,2,3,4,5)) doesn't throw an exception")
        except TypeError:
            pass

    ######################################################################
    def testIter(self):
        
        a = vec4(1.5, -2, 8.3, 2)
        b = []
        for i in a:
            b.append(i)

        self.failUnless(len(b)==4,
                        "Iteration nicht ueber 4 Elemente: %s"%str(b))
        self.failUnless(b[0]==1.5 and b[1]==-2 and b[2]==8.3 and b[3]==2,
                        "Iterationsfehler: %s"%str(b))

    ######################################################################
    def testCmp(self):
        a = vec4(1,2,3,4)
        b = vec4(5,6,7,8)

        self.failUnless(a<b,"<-Operation falsch (1)")
        self.failIf(b<a,"<-Operation falsch (2)")
        
        self.failUnless(b>a,">-Operation falsch (1)")
        self.failIf(b<a,">-Operation falsch (2)")

        self.failIf(a==b,"==-Operation falsch (1)")
        self.failUnless(a!=b,"!=-Operation falsch (1)")

        a.x=5
        self.failIf(a<b,"<-Operation falsch (3)")
        self.failIf(a>b,">-Operation falsch (3)")
        self.failUnless(a<=b,"<=-Operation falsch (1)")
        self.failIf(a>=b,">=-Operation falsch (1)")

        a.y=6
        a.z=7
        a.w=8
        self.failUnless(a==b, "==-Operation falsch (2)")
        self.failIf(a!=b, "!=-Operation falsch (2)")

    ######################################################################
    def testAdd(self):
        a = vec4(1.5, 2, 3, 4)
        b = vec4(2, -1.2, 7, -2)
        c=a+b
        self.failUnless(c==vec4(3.5,0.8,10,2), "falsche Addition: %s"%str(c))

    ######################################################################
    def testSub(self):
        a = vec4(1.5, 2, 3, 4)
        b = vec4(2, -1.2, 7, -2)
        c=a-b
        self.failUnless(c==vec4(-0.5,3.2,-4, 6),
                         "falsche Subtraktion: %s"%str(c))

    ######################################################################
    def testMul(self):
        a = vec4(-1, 2.5, 3, 2)
        c = 2*a
        self.failUnless(c==vec4(-2,5,6,4),
                         "falsche Multiplikation int*vec4: %s"%str(c))
        c = a*2
        self.failUnless(c==vec4(-2,5,6,4),
                         "falsche Multiplikation vec4*int: %s"%str(c))
        c = a*2
        self.failUnless(c==vec4(-2,5,6,4),
                         "falsche Multiplikation vec4*long: %s"%str(c))
        c = 2.0*a
        self.failUnless(c==vec4(-2,5,6,4),
                         "falsche Multiplikation float*vec4: %s"%str(c))

        b = vec4(2,4,-1,3)
        c = a*b
        self.failUnless(c==11.0,
                         "falsches Skalarprodukt: %s"%str(c))

        
    ######################################################################
    def testDiv(self):
        a = vec4(2,4,6,3)
        c = a/2.0
        self.failUnless(c==vec4(1,2,3,1.5),
                        "falsche Division: %s"%str(c))

        try:
            c = a/0
            self.fail("Division durch 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testMod(self):
        a = vec4(3,2.5,-1.8,3)
        c = a%2.0
        self.failUnless(c==vec4(1,0.5,0.2,1),
                        "Modulo1 falsch: %s"%str(c))

        b = vec4(3,2,2,2)
        c = a%b
        self.failUnless(c==vec4(0,0.5,0.2,1),
                        "Modulo2 falsch: %s"%str(c))

        try:
            c = a%0
            self.fail("Modulo mit 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

        try:
            c = a%vec4(0,0,0,0)
            self.fail("Modulo mit 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testNeg(self):
        a = vec4(1,-2,3,2)
        c = -a
        self.failUnless(c==vec4(-1,2,-3,-2),
                        "Negation falsch: %s"%str(c))

    ######################################################################
    def testPos(self):
        a = vec4(1,-2,3,2)
        c = +a
        self.failUnless(c==vec4(1,-2,3,2),
                        "Pos falsch: %s"%str(c))

    ######################################################################
    def testiAdd(self):
        a = vec4(1.5, 2, 3, 4)
        b = vec4(2, -1.2, 7, -2)
        a+=b
        self.failUnless(a==vec4(3.5,0.8,10,2), "falsche inline Addition: %s"%str(a))

    ######################################################################
    def testiSub(self):
        a = vec4(1.5, 2, 3, 4)
        b = vec4(2, -1.2, 7, -2)
        a-=b
        self.failUnless(a==vec4(-0.5,3.2,-4,6),
                        "falsche inline Subtraktion: %s"%str(a))

    ######################################################################
    def testiMul(self):
        a = vec4(1.5, 2, 3, 2)
        a*=2
        self.failUnless(a==vec4(3,4,6,4),
                        "falsche inline Multiplikation: %s"%str(a))

    ######################################################################
    def testiDiv(self):
        a = vec4(1.5, 2, 3, 2)
        a/=2.0
        self.failUnless(a==vec4(0.75,1,1.5,1),
                        "falsche inline Division: %s"%str(a))

    ######################################################################
    def testiMod(self):
        a = vec4(3,2.5,-1.8,3)
        a%=2.0
        self.failUnless(a==vec4(1.0,0.5,0.2,1.0),
                        "falsches inline Modulo1: %s"%str(a))

        a = vec4(3,2.5,-1.8,3)
        b = vec4(3,2,2,2)
        a%=b
        self.failUnless(a==vec4(0.0,0.5,0.2,1.0),
                        "falsches inline Modulo2: %s"%str(a))

    ######################################################################
    def testAbs(self):
        a = vec4(1, 2, 3, 4)
        c = abs(a)
        self.failUnless(c==math.sqrt(30),
                        "falsches abs(): %s"%str(c))

    ######################################################################
    def testLength(self):
        a = vec4(1, 2, 3, 4)
        c = a.length()
        self.failUnless(c==math.sqrt(30),
                        "falsches length(): %s"%str(c))

    ######################################################################
    def testGetAttr(self):
        a = vec4(1, 2, 3, 4)
        c = a.x
        self.failUnless(c==1,
                        "falsches vec4.x: %s"%str(c))

        c = a.y
        self.failUnless(c==2,
                        "falsches vec4.y: %s"%str(c))

        c = a.z
        self.failUnless(c==3,
                        "falsches vec4.z: %s"%str(c))

        c = a.w
        self.failUnless(c==4,
                        "falsches vec4.w: %s"%str(c))

        c = a.t
        self.failUnless(c==4,
                        "falsches vec4.t: %s"%str(c))

        try:
            c = a.k
            self.fail("Unbekanntes Attribut wirft keine Exception")
        except AttributeError:
            pass

    ######################################################################
    def testSetAttr(self):
        a = vec4(0,0,0,0)
        a.x = 1
        a.y = 2
        a.z = 3
        a.w = 4

        self.failUnless(a==vec4(1,2,3,4),
                        "falsches setAttr: %s"%str(a))

        a.t = 5

        self.failUnless(a==vec4(1,2,3,5),
                        "falsches setAttr: %s"%str(a))

#        try:
#            a.k = 2
#            self.fail("Unbekanntes Attribut (set) wirft keine Exception")
#        except AttributeError:
#            pass

    ######################################################################
    def testGetItem(self):
        a = vec4(1,2,3,4)

        c = a[0]
        self.failUnless(c==1,
                        "falsches vec4[0]: %s"%str(c))

        c = a[1]
        self.failUnless(c==2,
                        "falsches vec4[1]: %s"%str(c))

        c = a[2]
        self.failUnless(c==3,
                        "falsches vec4[2]: %s"%str(c))

        c = a[3]
        self.failUnless(c==4,
                        "falsches vec4[3]: %s"%str(c))

        try:
            c = a[-1]
            c = a[4]
            self.fail("vec4[-1] bzw. vec4[4] wirft keine Exception")
        except IndexError:
            pass

    ######################################################################
    def testSetItem(self):
        a = vec4(0,0,0,0)
        a[0] = 1
        a[1] = 2
        a[2] = 3
        a[3] = 4
        self.failUnless(a==vec4(1,2,3,4),
                        "falsches setItem: %s"%str(a))

        try:
            a[-1] = 1
            a[4]  = 1
            self.fail("vec4[-1] bzw. vec4[4] (set) wirft keine Exception")
        except IndexError:
            pass


    ######################################################################
    def testNormalize(self):
        a = vec4(1,0.5,-1.8,2)

        c = a.normalize()
        self.failUnless(c.length()==1.0,
                        "falsches normalize(): %s"%str(c))

        a = vec4(0,0,0,0)
        try:
            c = a.normalize()
            self.fail("0.normalize() wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testMinMax(self):
        a = vec4(1,-2,0.5,0.6)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 1)
        self.assertEqual(a.maxIndex(), 0)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 2)
        self.assertEqual(a.maxAbsIndex(), 1)

        a = vec4(0.6,1,-2,0.5)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 2)
        self.assertEqual(a.maxIndex(), 1)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 3)
        self.assertEqual(a.maxAbsIndex(), 2)

        a = vec4(0.5,0.6,1,-2)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 3)
        self.assertEqual(a.maxIndex(), 2)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 0)
        self.assertEqual(a.maxAbsIndex(), 3)

    ######################################################################
    def testPickle(self):
        fname = "delme_pickle.dat"
        
        f = file(fname, "wb")
        v = vec4(1, 2.5, -3.7, 12)
        pickle.dump(v, f)
        f.close()

        f = file(fname, "rb")
        w = pickle.load(f)
        f.close()

        os.remove(fname)

        self.assertEqual(v, w)


######################################################################

#cgtypes.setEpsilon(1E-12)

if __name__=="__main__":
    unittest.main()

