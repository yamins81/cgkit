# Test the mat3 class

import unittest
#from cgkit.light.cgtypes import *
from cgkit.cgtypes import *
import math, os, pickle, sys
from cgkit.sl import degrees, radians

class TestMat3(unittest.TestCase):

    def testConstructor(self):
        m = mat3(1,2,3,4,5,6,7,8,9)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Constructor (16 floats) failed")

        m = mat3(2.0)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[2.0,0,0, 0,2.0,0, 0,0,2.0],
                        "Constructor (1 float) failed")

        n = mat3(1,2,3,4,5,6,7,8,9)
        m = mat3(n)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Constructor (mat3) failed")

        m = mat3("1,2,3,4,5,6,7,8,9")
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Constructor (string9) failed")

        m = mat3([1,2,3,4,5,6,7,8,9])
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Constructor (list) failed")

        m = mat3((1,2,3), (4,5,6), (7,8,9))
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Constructor (3xtuple) failed")

        b1 = vec3(1,2,3)
        b2 = vec3(4,5,6)
        b3 = vec3(7,8,9)
        
        m = mat3(b1,b2,b3)
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Konstruktor (3xvec3) falsch")

        m = mat3([b1,b2,b3])
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "Konstruktor ([3xvec3]) falsch")

    ######################################################################
    def testCmp(self):
        M = mat3(1,2,3,4,5,6,7,8,9)
        N = mat3(1,2,3,4,5,6,7,8,9)

        self.failUnless(M==N, "mat3==mat3 (1) falsch")
        self.failIf(M!=N, "mat3!=mat3 (1) falsch")

        M = mat3(2,2,3,4,5,6,7,8,9)

        self.failUnless(M!=N, "mat3!=mat3 (2) falsch")
        self.failIf(M==N, "mat3==mat3 (2) falsch")

    ######################################################################
    def testAdd(self):
        M = mat3(1,2,3,4,5,6,7,8,9)
        N = mat3(1,2,3,4,5,6,7,8,9)

        C = M+N
        self.failUnless(C==mat3(2,4,6,8,10,12,14,16,18),
                        "mat3 Addition falsch")

    ######################################################################
    def testSub(self):
        M = mat3(2,4,6,8,10,12,14,16,18)
        N = mat3(1,2,3,4,5,6,7,8,9)

        C = M-N
        self.failUnless(C==N,
                        "mat3 Subtraktion falsch")

    ######################################################################
    def testMul(self):
        M = mat3(1,4,-2,3,5,7,2,0,3)
        N = mat3(8,6,3,-4,-1,2,3,-7,3)

        C = M*N
        self.failUnless(C==mat3(-14,16,5, 25,-36,40, 25,-9,15),
                        "mat3 Multiplikation falsch")


        C = 2.0*M
        self.failUnless(C==mat3(2,8,-4,6,10,14,4,0,6),
                        "mat3 Skalar-Multiplikation (links) falsch")

        C = M*2.0
        self.failUnless(C==mat3(2,8,-4,6,10,14,4,0,6),
                        "mat3 Skalar-Multiplikation (rechts) falsch")

        v = vec3(2,4,-1)
        c = M*v
        self.failUnless(c==vec3(20,19,1),
                        "mat3*vec3 falsch")

        c = v*M
        self.failUnless(c==vec3(12,28,21),
                        "vec3*mat3 falsch")


    ######################################################################
    def testDiv(self):
        M = mat3(2,4,6,8,10,12,14,16,18)
        N = mat3(1,2,3,4,5,6,7,8,9)

        C = M/2.0
        self.failUnless(C==N,
                        "mat3 Division falsch")

    ######################################################################
    def testMod(self):
        M = mat3(2,4,6,1.4,-1.8,12,1,1,1.8)

        self.failUnless(M%2.0==mat3(0,0,0,1.4,0.2,0,1,1,1.8),
                        "mat3 modulo (1) falsch")

        N = mat3(2,2,2,2,2,2,2,2,2)
        self.failUnless(M%N==mat3(0,0,0,1.4,0.2,0,1,1,1.8),
                        "mat3 modulo (2) falsch")

    ######################################################################
    def testNeg(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        C = -M
        self.failUnless(C==mat3(-1,-2,-3,-4,-5,-6,-7,-8,-9),
                        "mat3 Neg falsch")

    ######################################################################
    def testPos(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        C = +M
        self.failUnless(C==mat3(1,2,3,4,5,6,7,8,9),
                        "mat3 Pos falsch")

    ######################################################################
    def testIAdd(self):
        M = mat3(1,2,3,4,5,6,7,8,9)
        N = mat3(1,2,3,4,5,6,7,8,9)

        M+=N
        self.failUnless(M==mat3(2,4,6,8,10,12,14,16,18),
                        "mat3 inline Addition falsch")

    ######################################################################
    def testISub(self):
        M = mat3(2,4,6,8,10,12,14,16,18)
        N = mat3(1,2,3,4,5,6,7,8,9)

        M-=N
        self.failUnless(M==N,
                        "mat3 inline Subtraktion falsch")

    ######################################################################
    def testIMul(self):
        M = mat3(1,4,-2,3,5,7,2,0,3)
        N = mat3(8,6,3,-4,-1,2,3,-7,3)

        M*=N
        self.failUnless(M==mat3(-14,16,5, 25,-36,40, 25,-9,15),
                        "mat3 inline Multiplikation falsch")


        M = mat3(1,4,-2,3,5,7,2,0,3)
        M*=2.0
        self.failUnless(M==mat3(2,8,-4,6,10,14,4,0,6),
                        "mat3 inline Skalar-Multiplikation falsch")

    ######################################################################
    def testIDiv(self):
        M = mat3(2,4,6,8,10,12,14,16,18)
        N = mat3(1,2,3,4,5,6,7,8,9)

        M/=2.0
        self.failUnless(M==N,
                        "mat3 inline Division falsch")

    ######################################################################
    def testIMod(self):
        M = mat3(2,4,6,1.4,-1.8,12,1,1,1.8)

        M%=2.0
        self.failUnless(M==mat3(0,0,0,1.4,0.2,0,1,1,1.8),
                        "mat3 inline modulo (1) falsch")

        M = mat3(2,4,6, 1.4,-1.8,12, 1,1,1.8)
        N = mat3(2,2,2, 2,2,2, 2,2,2)
        M%=N
        self.failUnless(M==mat3(0,0,0,1.4,0.2,0,1,1,1.8),
                        "mat3 inline modulo (2) falsch")

    ######################################################################
    def testGetItem(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        v=M[0]
        self.failUnless(v==vec3(1,4,7), "mat3[0] falsch")
        v=M[1]
        self.failUnless(v==vec3(2,5,8), "mat3[1] falsch")
        v=M[2]
        self.failUnless(v==vec3(3,6,9), "mat3[2] falsch")

        self.failUnless(M[0,0]==1, "mat3[i,j] falsch")
        self.failUnless(M[0,1]==2, "mat3[i,j] falsch")
        self.failUnless(M[0,2]==3, "mat3[i,j] falsch")
        self.failUnless(M[1,0]==4, "mat3[i,j] falsch")
        self.failUnless(M[1,1]==5, "mat3[i,j] falsch")
        self.failUnless(M[1,2]==6, "mat3i,j] falsch")
        self.failUnless(M[2,0]==7, "mat3[i,j] falsch")
        self.failUnless(M[2,1]==8, "mat3[i,j] falsch")
        self.failUnless(M[2,2]==9, "mat3[i,j] falsch")

    ######################################################################
    def testSetItem(self):
        M = mat3(0)
        M[0]=vec3(1,4,7)
        M[1]=vec3(2,5,8)
        M[2]=vec3(3,6,9)
        
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        M = mat3(0)
        M[0]=(1,4,7)
        M[1]=(2,5,8)
        M[2]=(3,6,9)
        
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        M = mat3(0)
        M[0,0] = 1
        M[0,1] = 2
        M[0,2] = 3
        M[1,0] = 4
        M[1,1] = 5
        M[1,2] = 6
        M[2,0] = 7
        M[2,1] = 8
        M[2,2] = 9

        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

    ######################################################################
    def testGetRow(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        v=M.getRow(0)
        self.failUnless(v==vec3(1,2,3), "mat3.getRow(0) falsch")
        v=M.getRow(1)
        self.failUnless(v==vec3(4,5,6), "mat3.getRow(1) falsch")
        v=M.getRow(2)
        self.failUnless(v==vec3(7,8,9), "mat3.getRow(2) falsch")

        try:
            v=M.getRow(-1)
            v=M.getRow(3)
            self.fail("getRow() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.getRow("2")
            self.fail("getRow(string) wirft keine Exception")
        except TypeError:
            pass

    ######################################################################
    def testSetRow(self):
        M = mat3(0)
        M.setRow(0,vec3(1,2,3))
        M.setRow(1,vec3(4,5,6))
        M.setRow(2,vec3(7,8,9))
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        M = mat3(0)
        M.setRow(0,(1,2,3))
        M.setRow(1,(4,5,6))
        M.setRow(2,(7,8,9))
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        try:
            M.setRow(-1,vec3(1,2,3))
            M.setRow(3,vec3(1,2,3))
            self.fail("setRow() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.setRow("2", vec3(1,2,3))
            self.fail("setRow(string) wirft keine Exception")
        except TypeError:
            pass

        try:
            v=M.setRow(0, vec4(1,2,3))
            self.fail("setRow(0,vec4) wirft keine Exception")
        except:  # How can I catch the ArgumentError exception?
            pass

    ######################################################################
    def testGetColumn(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        v=M.getColumn(0)
        self.failUnless(v==vec3(1,4,7), "mat3.getColumn(0) falsch")
        v=M.getColumn(1)
        self.failUnless(v==vec3(2,5,8), "mat3.getColumn(1) falsch")
        v=M.getColumn(2)
        self.failUnless(v==vec3(3,6,9), "mat3.getColumn(2) falsch")

        try:
            v=M.getColumn(-1)
            v=M.getColumn(3)
            self.fail("getColumn() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.getColumn("2")
            self.fail("getColumn(string) wirft keine Exception")
        except TypeError:
            pass

    ######################################################################
    def testSetColumn(self):
        M = mat3(0)
        M.setColumn(0,vec3(1,4,7))
        M.setColumn(1,vec3(2,5,8))
        M.setColumn(2,vec3(3,6,9))
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        M = mat3(0)
        M.setColumn(0,(1,4,7))
        M.setColumn(1,(2,5,8))
        M.setColumn(2,(3,6,9))
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

        try:
            M.setColumn(-1,vec3(1,2,3))
            M.setColumn(3,vec3(1,2,3))
            self.fail("setColumn() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.setColumn("2", vec4(1,2,3))
            self.fail("setColumn(string) wirft keine Exception")
        except TypeError:
            pass

        try:
            v=M.setColumn(0, vec4(1,2,3))
            self.fail("setColumn(0,vec4) wirft keine Exception")
        except:  # How can I catch the ArgumentError exception?
            pass

    ######################################################################
    def testDiag(self):
        M = mat3(1,2,3,
                 4,5,6,
                 7,8,9)
        self.assertEqual(M.getDiag(), vec3(1,5,9))
        M.setDiag(vec3(-1,-5,-9))
        self.assertEqual(M, mat3(-1,2,3,4,-5,6,7,8,-9))
        M.setDiag((1,5,9))
        self.assertEqual(M, mat3(1,2,3,4,5,6,7,8,9))

    ######################################################################
    def testToList(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        a = M.toList()
        self.failUnless(a==[1,4,7, 2,5,8, 3,6,9],
                        "mat3.toList() falsch")

        a = M.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9],
                        "mat3.toList() falsch")

    ######################################################################
    def testIdentity(self):
        M = mat3().identity()
        self.assertEqual(M,mat3(1,0,0, 0,1,0, 0,0,1))

        M = mat3.identity()
        self.assertEqual(M,mat3(1,0,0, 0,1,0, 0,0,1))

    ######################################################################
    def testTranspose(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        N = M.transpose()
        self.failUnless(N==mat3(1,4,7, 2,5,8, 3,6,9),
                        "mat3.transpose() falsch")

    ######################################################################
    def testDet(self):
        M = mat3().rotation(0.5, vec3(1,1,1))

        d = round(M.determinant(), 10)
        self.failUnless(d==1.0,
                        "mat3.determinant() (1) falsch")

        M[0] = -M[0]
        d = round(M.determinant(), 10)
        self.failUnless(d==-1.0,
                        "mat3.determinant() (2) falsch")

    ######################################################################
    def testInverse(self):
        M = mat3().rotation(0.5, vec3(1,1,1))
        N = mat3().rotation(-0.5, vec3(1,1,1))

        Mi = M.inverse()
        self.failUnless(Mi==N,
                        "mat3.inverse() (1) falsch")

        M = mat3(1,2,3,5,-1,2,3,1,-5)
        Mi = M.inverse()
        self.failUnless(M*Mi==mat3(1),
                        "mat3.inverse() (2) falsch")

        M[1]=M[2]
        try:
            Mi = M.inverse()
            self.fail("mat3.inverse() wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testScaling(self):
        M = mat3().scaling(vec3(2,3,-2))
        self.assertEqual(M, mat3(2,0,0, 0,3,0, 0,0,-2))

        M = mat3.scaling(vec3(2,3,-2))
        self.assertEqual(M, mat3(2,0,0, 0,3,0, 0,0,-2))

        M = mat3.scaling((2,3,-2))
        self.assertEqual(M, mat3(2,0,0, 0,3,0, 0,0,-2))

    ######################################################################
    def testScale(self):
        M = mat3(1).scale(vec3(2,3,-2))
        self.assertEqual(M, mat3(2,0,0, 0,3,0, 0,0,-2))

        M = mat3(1).scale((2,3,-2))
        self.assertEqual(M, mat3(2,0,0, 0,3,0, 0,0,-2))

    ######################################################################
    def testRotation(self):
        M = mat3().rotation(math.pi/2, vec3(0,0,1))
        self.assertEqual(M, mat3(0,-1,0, 1,0,0, 0,0,1))

        M = mat3.rotation(math.pi/2, (0,0,1))
        self.assertEqual(M, mat3(0,-1,0, 1,0,0, 0,0,1))

    ######################################################################
    def testRotate(self):
        M = mat3(1).rotate(math.pi/2, vec3(0,0,1))
        self.assertEqual(M, mat3(0,-1,0, 1,0,0, 0,0,1))

        M = mat3(1).rotate(math.pi/2, (0,0,1))
        self.assertEqual(M, mat3(0,-1,0, 1,0,0, 0,0,1))

    ######################################################################
    def testOrtho(self):
        M = mat3(1,2,3,4,5,6,7,8,9)

        C = M.ortho()
        b1 = C[0]
        b2 = C[1]
        b3 = C[2]
        a = vec3(b1*b2, b2*b3, b1*b3)
        self.failUnless(a==vec3(0,0,0),
                        "mat3 ortho falsch")

    ######################################################################
    def testDecompose(self):
        S = mat3().scaling(vec3(2,3,2))
        R = mat3().rotation(0.5, vec3(1,0.2,1))
        M = R*S

        r,s = M.decompose()
        self.failUnless(s==vec3(2,3,2), "mat3 decompose: S falsch")
        self.failUnless(r==R, "mat3 decompose: R falsch")
        N=mat3(1)
        N=N*R
        N=N.scale(s)
        self.failUnless(N==M, "mat3 decompose: Ergebnis falsch")

    ######################################################################
    def testEuler(self):
        """Test the fromEuler*() and toEuler*() methods.

        The result from an fromEuler*() method is compared to an equivalent
        matrix that is composed by 3 individual rotations.
        """

        angles = [{"X":radians(20), "Y":radians(30), "Z":radians(40)},
                  {"X":radians(0), "Y":radians(0), "Z":radians(0)},
                  {"X":radians(350), "Y":radians(0), "Z":radians(0)},
                  {"X":radians(0), "Y":radians(350), "Z":radians(0)},
                  {"X":radians(0), "Y":radians(0), "Z":radians(350)},]
        axis = {"X":vec3(1,0,0), "Y":vec3(0,1,0), "Z":vec3(0,0,1)}

        for order in ["XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX"]:
            for angle in angles:
                R1 = mat3.rotation(angle[order[0]], axis[order[0]])
                R2 = mat3.rotation(angle[order[1]], axis[order[1]])
                R3 = mat3.rotation(angle[order[2]], axis[order[2]])
                # Each rotation is about the *global* axis, so these rotations
                # have to be applied just in the opposite order than mentioned
                # in the fromEuler*() method name.
                C = R1*R2*R3
                exec ('E = mat3.fromEuler%s(angle["X"], angle["Y"], angle["Z"])'%order)
                self.assertEqual(E, C)
    
                exec ('x,y,z = E.toEuler%s()'%order)
                exec ('E2 = mat3.fromEuler%s(x, y, z)'%order)
                if E2!=E:
#                    print E
#                    print E2
                    msg = "The matrix E2 generated from the toEuler() angles doesn't match the original matrix E.\n"
                    msg += "Original angles: (%s, %s, %s), toEuler angles: (%s, %s, %s)"%(degrees(angle["X"]),degrees(angle["Y"]),degrees(angle["Z"]),degrees(x),degrees(y),degrees(z))
                    self.fail(msg)

    ######################################################################
    def testFromToRotation(self):
        R = mat3.fromToRotation((1,0,0), (0,1,0))
        self.assertEqual(R.determinant(), 1.0)
        self.assertEqual(R, mat3(0,-1,0, 1,0,0, 0,0,1))

        R = mat3.fromToRotation(vec3(1,0,0), vec3(0,1,0))
        self.assertEqual(R.determinant(), 1.0)
        self.assertEqual(R, mat3(0,-1,0, 1,0,0, 0,0,1))

        a = vec3(1,1,0).normalize()
        b = vec3(-1,0.5,2).normalize()
        R = mat3.fromToRotation(a, b)
        self.assertAlmostEqual(R.determinant(), 1.0, 8)
        self.assertEqual(R*a, b)

        a = vec3(1,1,0).normalize()
        b = vec3(1,1,0.000001).normalize()
        R = mat3.fromToRotation(a, b)
        self.assertAlmostEqual(R.determinant(), 1.0, 8)
        self.assertEqual(R*a, b)

    ######################################################################
    def testPickle(self):
        fname = "delme_pickle.dat"
        
        f = open(fname, "wb")
        m = mat3(1,2,3,4,5,6,7,8,9)
        pickle.dump(m, f)
        f.close()

        f = open(fname, "rb")
        n = pickle.load(f)
        f.close()

        os.remove(fname)

        self.assertEqual(m, n)

######################################################################

setEpsilon(1E-12)

if __name__=="__main__":
    unittest.main()
    
