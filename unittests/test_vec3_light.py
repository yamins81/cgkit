# Test the vec3 class

import unittest
#from cgkit.cgtypes import *
from cgkit.light.cgtypes import *
import math, os, pickle

class TestVec3(unittest.TestCase):
    
    def setUp(self):
        self.a = vec3(1.5, 2, 3)
        self.b = vec3(2, -1.2, 7)
        self.c = vec3()

    ######################################################################
    def testConstructor(self):
        v = vec3(1.5,-2,3)
        self.failUnless(v.x==1.5 and v.y==-2.0 and v.z==3,
                        "vec3(1.5,-2,3) != %s"%str(v))

        v = vec3()
        self.failUnless(v==vec3(0,0,0), "vec3() != (0,0,0) : %s"%str(v))

        v = vec3(2.5)
        self.failUnless(v==vec3(2.5,2.5,2.5),
                        "vec3(2.5) != vec3(2.5,2.5,2.5) : %s"%str(v))

        v = vec3(-1.5, 42)
        self.failUnless(v==vec3(-1.5, 42.0, 0.0),
                        "vec3(-1.5, 42) != vec3(-1.5, 42.0, 0.0) : %s"%str(v))

        v = vec3(())
        self.failUnless(v==vec3(0, 0, 0),
                        "vec3(()) != vec3(0,0,0) : %s"%str(v))

        v = vec3([1])
        self.failUnless(v==vec3(1, 1, 1),
                        "vec3([1]) != vec3(1,1,1) : %s"%str(v))

        v = vec3((1,2))
        self.failUnless(v==vec3(1, 2, 0),
                        "vec3((1,2)) != vec3(1,2,0) : %s"%str(v))

        v = vec3((1,2,3))
        self.failUnless(v==vec3(1, 2, 3),
                        "vec3((1,2,3)) != vec3(1,2,3) : %s"%str(v))

        w = vec3(7,-2,3)
        v = vec3(w)
        self.failUnless(v==vec3(7, -2, 3),
                        "vec3(vec3) : %s"%str(v))
        w.x = 12
        self.failUnless(v==vec3(7, -2, 3),
                        "vec3(vec3) doesn't create a copy: %s"%str(v))

        v = vec3("1,2,3")
        self.failUnless(v==vec3(1, 2, 3),
                        'vec3("1,2,3") != vec3(1,2,3) : %s'%str(v))

        v = vec3("1,2")
        self.failUnless(v==vec3(1, 2, 0),
                        'vec3("1,2") != vec3(1,2,0) : %s'%str(v))

        v = vec3("1")
        self.failUnless(v==vec3(1, 1, 1),
                        'vec3("1") != vec3(1,1,1) : %s'%str(v))

        v = vec3("")
        self.failUnless(v==vec3(0, 0, 0),
                        'vec3("") != vec3(0,0,0) : %s'%str(v))

        try:
            v = vec3("x,y,z")
            self.fail("vec3('x,y,z') doesn't throw an exception")
        except ValueError:
            pass

        try:
            v = vec3(1,2,3,4)
            self.fail("vec3(1,2,3,4) doesn't throw an exception")
        except TypeError:
            pass

        try:
            v = vec3((1,2,3,4))
            self.fail("vec3((1,2,3,4)) doesn't throw an exception")
        except TypeError:
            pass

    ######################################################################
    def testIter(self):
        a = vec3(1.5, -2, 8.3)
        b = []
        for i in a:
            b.append(i)

        self.failUnless(len(b)==3,
                        "Iteration should result in 3 values: %s"%str(b))
        self.failUnless(b[0]==1.5 and b[1]==-2 and b[2]==8.3,
                        "Iteration error: %s"%str(b))

    ######################################################################
    def testCmp(self):
        a = vec3(1,2,3)
        b = vec3(5,6,7)

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
        self.failUnless(a==b, "==-Operation falsch (2)")
        self.failIf(a!=b, "!=-Operation falsch (2)")

    ######################################################################
    def testAdd(self):
        a = vec3(1.5, 2, 3)
        b = vec3(2, -1.2, 7)
        c=a+b
        self.failUnless(c==vec3(3.5,0.8,10), "falsche Addition: %s"%str(c))

    ######################################################################
    def testSub(self):
        a = vec3(1.5, 2, 3)
        b = vec3(2, -1.2, 7)
        c=a-b
        self.failUnless(c==vec3(-0.5,3.2,-4),
                         "falsche Subtraktion: %s"%str(c))

    ######################################################################
    def testMul(self):
        a = vec3(-1, 2.5, 3)
        c = 2*a
        self.failUnless(c==vec3(-2,5,6),
                         "falsche Multiplikation int*vec3: %s"%str(c))
        c = a*2
        self.failUnless(c==vec3(-2,5,6),
                         "falsche Multiplikation vec3*int: %s"%str(c))
        c = a*2
        self.failUnless(c==vec3(-2,5,6),
                         "falsche Multiplikation vec3*long: %s"%str(c))
        c = 2.0*a
        self.failUnless(c==vec3(-2,5,6),
                         "falsche Multiplikation float*vec3: %s"%str(c))

        b = vec3(2,4,-1)
        c = a*b
        self.failUnless(c==5.0,
                         "falsches Skalarprodukt: %s"%str(c))

        
    ######################################################################
    def testDiv(self):
        a = vec3(2,4,6)
        c = a/2.0
        self.failUnless(c==vec3(1,2,3),
                        "falsche Division: %s"%str(c))

        try:
            c = a/0
            self.fail("Division durch 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testMod(self):
        a = vec3(3,2.5,-1.8)
        c = a%2.0
        self.assertEqual(c, vec3(1,0.5,0.2))

        b = vec3(3,2,2)
        c = a%b
        self.assertEqual(c, vec3(0,0.5,0.2))


        try:
            c = a%0
            self.fail("Modulo mit 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

        try:
            c = a%vec3(0,0,0)
            self.fail("Modulo mit 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testNeg(self):
        a = vec3(1,-2,3)
        c = -a
        self.failUnless(c==vec3(-1,2,-3),
                        "Negation falsch: %s"%str(c))

    ######################################################################
    def testPos(self):
        a = vec3(1,-2,3)
        c = +a
        self.failUnless(c==vec3(1,-2,3),
                        "Pos falsch: %s"%str(c))

    ######################################################################
    def testiAdd(self):
        a = vec3(1.5, 2, 3)
        b = vec3(2, -1.2, 7)
        a+=b
        self.failUnless(a==vec3(3.5,0.8,10), "falsche inline Addition: %s"%str(a))

    ######################################################################
    def testiSub(self):
        a = vec3(1.5, 2, 3)
        b = vec3(2, -1.2, 7)
        a-=b
        self.failUnless(a==vec3(-0.5,3.2,-4),
                        "falsche inline Subtraktion: %s"%str(a))

    ######################################################################
    def testiMul(self):
        a = vec3(1.5, 2, 3)
        a*=2
        self.failUnless(a==vec3(3,4,6),
                        "falsche inline Multiplikation: %s"%str(a))

    ######################################################################
    def testiDiv(self):
        a = vec3(1.5, 2, 3)
        a/=2.0
        self.failUnless(a==vec3(0.75,1,1.5),
                        "falsche inline Division: %s"%str(a))

    ######################################################################
    def testiMod(self):
        a = vec3(3,2.5,-1.8)
        a%=2.0
        self.failUnless(a==vec3(1.0,0.5,0.2),
                        "falsches inline Modulo1: %s"%str(a))

        a = vec3(3,2.5,-1.8)
        b = vec3(3,2,2)
        a%=b
        self.failUnless(a==vec3(0.0,0.5,0.2),
                        "falsches inline Modulo2: %s"%str(a))

    ######################################################################
    def testAbs(self):
        a = vec3(1, 2, 3)
        c = abs(a)
        self.assertEqual(c, math.sqrt(14))

    ######################################################################
    def testLength(self):
        a = vec3(1, 2, 3)
        c = a.length()
        self.failUnless(c==math.sqrt(14),
                        "falsches length(): %s"%str(c))

    ######################################################################
    def testGetAttr(self):
        a = vec3(1, 2, 3)
        c = a.x
        self.failUnless(c==1,
                        "falsches vec3.x: %s"%str(c))

        c = a.y
        self.failUnless(c==2,
                        "falsches vec3.y: %s"%str(c))

        c = a.z
        self.failUnless(c==3,
                        "falsches vec3.z: %s"%str(c))

        try:
            c = a.k
            self.fail("Unbekanntes Attribut wirft keine Exception")
        except AttributeError:
            pass

    ######################################################################
    def testSetAttr(self):
        a = vec3(0,0,0)
        a.x = 1
        a.y = 2
        a.z = 3

        self.failUnless(a==vec3(1,2,3),
                        "falsches setAttr: %s"%str(a))

#        try:
#            a.k = 2
#            self.fail("Unbekanntes Attribut (set) wirft keine Exception")
#        except AttributeError:
#            pass

    ######################################################################
    def testGetItem(self):
        a = vec3(1,2,3)

        c = a[0]
        self.failUnless(c==1,
                        "falsches vec3[0]: %s"%str(c))

        c = a[1]
        self.failUnless(c==2,
                        "falsches vec3[1]: %s"%str(c))

        c = a[2]
        self.failUnless(c==3,
                        "falsches vec3[2]: %s"%str(c))

        try:
            c = a[-1]
            c = a[3]
            self.fail("vec3[-1] bzw. vec3[3] wirft keine Exception")
        except IndexError:
            pass

    ######################################################################
    def testSetItem(self):
        a = vec3(0,0,0)
        a[0] = 1
        a[1] = 2
        a[2] = 3
        self.failUnless(a==vec3(1,2,3),
                        "falsches setItem: %s"%str(a))

        try:
            a[-1] = 1
            a[3]  = 1
            self.fail("vec3[-1] bzw. vec3[3] (set) wirft keine Exception")
        except IndexError:
            pass

    ######################################################################
    def testCross(self):
        a = vec3(1,0.5,-1.8)
        b = vec3(-0.3, 0.75, 0.5)
        c = a.cross(b)

        oldeps = setEpsilon(1E-10)
        self.failUnless(c==vec3(1.6, 0.04, 0.9),
                        "falsches cross(): %s"%str(c))
        setEpsilon(oldeps)

    ######################################################################
    def testNormalize(self):
        a = vec3(1,0.5,-1.8)

        c = a.normalize()
        self.failUnless(c.length()==1.0,
                        "falsches normalize(): %s"%str(c))

        a = vec3(0,0,0)
        try:
            c = a.normalize()
            self.fail("0.normalize() wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testAngle(self):
        a = vec3(0,0,1)
        b = vec3(2,0,0)

        c = a.angle(b)
        self.assertEqual(c, math.pi/2)

        c = b.angle(tuple(a))
        self.assertEqual(c, math.pi/2)

        c = a.angle(a)
        self.assertEqual(c, 0.0)

        a = vec3(1,0,0)
        b = vec3(1,1,0)

        c = a.angle(b)
        self.failUnless(abs(c-math.pi/4)<1E-10,
                        "falsches angle() (4): %s"%str(c))

        try:
            n = vec3(0,0,0)
            c = a.angle(n)
            c = n.angle(a)
            self.fail("angle mit 0 wirft keine Exception")
        except ZeroDivisionError:
            pass
        
    ######################################################################
    def testReflect(self):
        a = vec3(1,-1,0)
        n = vec3(0,1,0)

        c = a.reflect(n)
        self.assertEqual(c, vec3(1,1,0))

        c = a.reflect((0,1,0))
        self.assertEqual(c, vec3(1,1,0))

    ######################################################################
    def testRefract(self):
        a = vec3(1,-1,0)
        n = vec3(0,1,0)

        c = a.refract(n,1.5)
        self.assertEqual(c, vec3(1.5,-1,0))

        c = a.refract((0,1,0),1.5)
        self.assertEqual(c, vec3(1.5,-1,0))

    ######################################################################
    def testOrtho(self):
        a = vec3(1,-1,0.5)

        c = a.ortho()
        self.failUnless(a*c==0.0,
                        "falsches ortho(): %s"%str(c))

    ######################################################################
    def testMinMax(self):
        a = vec3(1,-2,0.5)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 1)
        self.assertEqual(a.maxIndex(), 0)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 2)
        self.assertEqual(a.maxAbsIndex(), 1)

        a = vec3(0.5,1,-2)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 2)
        self.assertEqual(a.maxIndex(), 1)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 0)
        self.assertEqual(a.maxAbsIndex(), 2)

        a = vec3(-2,0.5,1)
        self.assertEqual(a.min(), -2)
        self.assertEqual(a.max(), 1)
        self.assertEqual(a.minIndex(), 0)
        self.assertEqual(a.maxIndex(), 2)
        self.assertEqual(a.minAbs(), 0.5)
        self.assertEqual(a.maxAbs(), 2)
        self.assertEqual(a.minAbsIndex(), 1)
        self.assertEqual(a.maxAbsIndex(), 0)


    ######################################################################
    def testPickle(self):
        fname = "delme_pickle.dat"
        
        f = open(fname, "wb")
        v = vec3(1, 2.5, -3.7)
        pickle.dump(v, f)
        f.close()

        f = open(fname, "rb")
        w = pickle.load(f)
        f.close()

        os.remove(fname)

        self.assertEqual(v, w)

######################################################################

#cgtypes.setEpsilon(1E-12)

if __name__=="__main__":
    unittest.main()

