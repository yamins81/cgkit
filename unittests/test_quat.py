# Test the quat class

import unittest
from cgkit.cgtypes import *
#from cgkit.light.cgtypes import *
import math, os, pickle

class TestQuat(unittest.TestCase):

    def testConstructor(self):
        q = quat(1.5,-2,3,2)
        self.failUnless(q.w==1.5 and q.x==-2.0 and q.y==3 and q.z==2,
                        "Konstruktor quat(w,x,y,z) falsch: %s"%str(q))

        q = quat()
        self.failUnless(q==quat(0,0,0,0),"Konstruktor quat() falsch: %s"%str(q))

        q = quat(2.5)
        self.failUnless(q==quat(2.5,0,0,0),
                        "Konstruktor quat(x) falsch: %s"%str(q))


        w = quat(7,-2,3,2)
        q = quat(w)
        self.failUnless(q==quat(7, -2, 3, 2),
                        "Konstruktor quat(quat) falsch: %s"%str(q))
        w.x = 12
        self.failUnless(q==quat(7, -2, 3, 2),
                        "Konstruktor quat(quat) legt keine Kopie an: %s"%str(q))

        q = quat("1.2")
        self.failUnless(q==quat(1.2, 0, 0, 0),
                        "Konstruktor quat(string2) falsch: %s"%str(q))

        v = quat("")
        self.failUnless(v==quat(0, 0, 0, 0),
                        "Konstruktor quat(string0) falsch: %s"%str(q))

        try:
            v = quat("x,y,z,w")
            self.fail("quat('x,y,z,w') wirft keine Exception")
        except ValueError:
            pass

        try:
            v = quat(1,2,3,4,5)
            self.fail("quat(x,y,z,s,t) wirft keine Exception")
        except TypeError:
            pass

        try:
            v = quat((1,2,3,4,5))
            self.fail("quat((x,y,z,s,t)) wirft keine Exception")
        except TypeError:
            pass


    ######################################################################
    def testCmp(self):
        a = quat(1,2,3,4)
        b = quat(5,6,7,8)

        self.failIf(a==b,"==-Operation falsch (1)")
        self.failUnless(a!=b,"!=-Operation falsch (1)")

        a.w=5
        a.x=6
        a.y=7
        a.z=8
        self.failUnless(a==b, "==-Operation falsch (2)")
        self.failIf(a!=b, "!=-Operation falsch (2)")

    ######################################################################
    def testAdd(self):
        a = quat(1.5, 2, 3, 4)
        b = quat(2, -1.2, 7, -2)
        c=a+b
        self.failUnless(c==quat(3.5,0.8,10,2), "falsche Addition: %s"%str(c))

    ######################################################################
    def testSub(self):
        a = quat(1.5, 2, 3, 4)
        b = quat(2, -1.2, 7, -2)
        c=a-b
        self.failUnless(c==quat(-0.5,3.2,-4, 6),
                         "falsche Subtraktion: %s"%str(c))

    ######################################################################
    def testMul(self):
        a = quat(-1, 2.5, 3, 2)
        c = 2*a
        self.failUnless(c==quat(-2,5,6,4),
                         "falsche Multiplikation int*quat: %s"%str(c))
        c = a*2
        self.failUnless(c==quat(-2,5,6,4),
                         "falsche Multiplikation quat*int: %s"%str(c))
        c = a*2
        self.failUnless(c==quat(-2,5,6,4),
                         "falsche Multiplikation quat*long: %s"%str(c))
        c = 2.0*a
        self.failUnless(c==quat(-2,5,6,4),
                         "falsche Multiplikation float*quat: %s"%str(c))

#        b = quat(2,4,-1,3)
#        c = a*b
#        self.failUnless(c==11.0,
#                         "falsches Skalarprodukt: %s"%str(c))

        
    ######################################################################
    def testDiv(self):
        a = quat(2,4,6,3)
        c = a/2.0
        self.failUnless(c==quat(1,2,3,1.5),
                        "falsche Division: %s"%str(c))

        try:
            c = a/0
            self.fail("Division durch 0 wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testPow(self):
        a = quat(1,-2,3,2).normalize()
        a = quat(a)
        self.failUnless(a*a==a**2,
                        "pow falsch: a*a != a**2")

        self.failUnless(a*a==pow(a,2),
                        "pow falsch: a*a != a**2")


    ######################################################################
    def testNeg(self):
        a = quat(1,-2,3,2)
        c = -a
        self.failUnless(c==quat(-1,2,-3,-2),
                        "Negation falsch: %s"%str(c))

    ######################################################################
    def testPos(self):
        a = quat(1,-2,3,2)
        c = +a
        self.failUnless(c==quat(1,-2,3,2),
                        "Pos falsch: %s"%str(c))

    ######################################################################
    def testiAdd(self):
        a = quat(1.5, 2, 3, 4)
        b = quat(2, -1.2, 7, -2)
        a+=b
        self.assertEqual(a, quat(3.5,0.8,10,2))

    ######################################################################
    def testiSub(self):
        a = quat(1.5, 2, 3, 4)
        b = quat(2, -1.2, 7, -2)
        a-=b
        self.assertEqual(a, quat(-0.5,3.2,-4,6))

    ######################################################################
    def testiMul(self):
        a = quat(1.5, 2, 3, 2)
        a*=2
        self.assertEqual(a, quat(3,4,6,4))

    ######################################################################
    def testiDiv(self):
        a = quat(1.5, 2, 3, 2)
        a/=2.0
        self.failUnless(a==quat(0.75,1,1.5,1),
                        "falsche inline Division: %s"%str(a))


    ######################################################################
    def testAbs(self):
        a = quat(1, 2, 3, 4)
        c = abs(a)
        self.failUnless(c==math.sqrt(30),
                        "falsches abs(): %s"%str(c))

    ######################################################################
    def testConjugate(self):
        a = quat(1, 2, 3, 4)
        c = a.conjugate()
        self.failUnless(c==quat(1,-2,-3,-4),
                        "falsches conjugate(): %s"%str(c))


    ######################################################################
    def testNormalize(self):
        a = quat(1,0.5,-1.8,2)

        c = a.normalize()
        self.failUnless(abs(c)==1.0,
                        "falsches normalize(): %s"%str(c))

        a = vec4(0,0,0,0)
        try:
            c = a.normalize()
            self.fail("0.normalize() wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testInverse(self):
        a = quat(1,0.5,-1.8,2)

        ai = a.inverse()
        self.failUnless(a*ai==quat(1),
                        "quat.inverse() falsch")

    ######################################################################
    def testAngleAxisMat(self):

        # fromAngleAxis (0 rotation)
        q = quat().fromAngleAxis(0, vec3(1,1,1))
        self.assertEqual(q, quat(1,0,0,0))

        q = quat().fromAngleAxis(0.5, vec3(0,0,0))
        self.assertEqual(q, quat(1,0,0,0))

        # fromAngleAxis (check if the length of the axis is irrelevant)
        q1 = quat().fromAngleAxis(0.3, vec3(0.3,0.8,-0.2))
        q2 = quat().fromAngleAxis(0.3, 2.5*vec3(0.3,0.8,-0.2))
        self.assertEqual(q1,q2)
        
        # fromAngleAxis/toAngleAxis
        q = quat().fromAngleAxis(0.5, vec3(1,0,0))
        a,axis = q.toAngleAxis()
        self.assertEqual((round(a,10),axis), (0.5, vec3(1,0,0)))

        # fromAngleAxis/toAngleAxis
        q = quat().fromAngleAxis(0.3, vec3(0.3,0.8,-0.2))
        a,axis = q.toAngleAxis()
        self.assertEqual((round(a,10),axis), (0.3, vec3(0.3,0.8,-0.2).normalize()))

        # fromMat/toMat3
        m = mat3().rotation(0.5, vec3(0,1,0))
        q = quat().fromMat(mat3().rotation(0.5,vec3(0,1,0)))
        n = q.toMat3()
        self.failUnless(m==n, "fromMat/toMat invalid")

    ######################################################################
    def testDot(self):
        a = quat(1,-2,3,2)
        b = quat(3,0.5,-2,1)
        c = a.dot(b)
        self.failUnless(c==-2.0,
                        "dot() falsch: %f != -2"%c)
     
    ######################################################################
    def testSlerp(self):
        a = quat(1,-2,3,2).normalize()
        b = quat(-3,-0.5,2,-1).normalize()

        self.assertEqual(slerp(0,a,b), a)

        self.assertEqual(slerp(1,a,b),b)

        c = quat(-0.37269829371452667, -0.4025936484158687, 0.8246578256709034, 0.13767282475934825)
        self.assertEqual(slerp(0.5,a,b,shortest=False), c)

    ######################################################################
    def testSquad(self):
        a = quat(1,-2,3,2).normalize()
        b = quat(0.2,0.5,2,1).normalize()
        c = quat(-1,1,2,-2).normalize()
        d = quat(-3,-0.5,2,-1).normalize()

        self.assertEqual(squad(0,a,b,c,d), a)

        self.assertEqual(squad(1,a,b,c,d), d)

        sq = quat(-0.281512430255, -0.0403661020814, 0.958668907982, 0.0086749225996)
        self.assertEqual(squad(0.5,a,b,c,d), sq)

    ######################################################################
    def testRotateVec(self):
        """Test quat.rotateVec()
        """
        s = math.sqrt(2)/2.0
        q = quat(0.25*math.pi, (1,0,0))
        v = vec3(0,1,0)
        self.assertEqual(q.rotateVec(v), vec3(0,s,s))

        m = mat3.rotation(0.25*math.pi, (1,0,0))
        self.assertEqual(q.rotateVec(tuple(v)), m*v)

        m = q.toMat3()
        self.assertEqual(q.rotateVec(v), m*v)

    ######################################################################
    def testPickle(self):
        fname = "delme_pickle.dat"
        
        f = open(fname, "wb")
        q = quat(1,2,3,4)
        pickle.dump(q, f)
        f.close()

        f = open(fname, "rb")
        w = pickle.load(f)
        f.close()

        os.remove(fname)

        self.assertEqual(q, w)


######################################################################

setEpsilon(1E-12)

if __name__=="__main__":
    unittest.main()
