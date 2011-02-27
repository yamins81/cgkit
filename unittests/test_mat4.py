# Test the mat4 class

import unittest
from cgkit.cgtypes import *
#from cgkit.light.cgtypes import *
import math, os, pickle, sys, copy

class TestMat4(unittest.TestCase):

    def testConstructor(self):
        m = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (16 floats) falsch")

        m = mat4(2.0)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[2.0,0,0,0, 0,2.0,0,0, 0,0,2.0,0, 0,0,0,2.0],
                        "Konstruktor (1 float) falsch")

        n = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        m = mat4(n)
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (mat4) falsch")

        m = mat4("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16")
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (string16) falsch")

        m = mat4([1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16])
        a = m.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (list) falsch")

        m = mat4((1,2,3,4), (5,6,7,8), (9,10,11,12), (13,14,15,16))
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (4xtuple) falsch")

        b1 = vec4(1,2,3,4)
        b2 = vec4(5,6,7,8)
        b3 = vec4(9,10,11,12)
        b4 = vec4(13,14,15,16)
        
        m = mat4(b1,b2,b3,b4)
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor (4xvec4) falsch")

        m = mat4([b1,b2,b3,b4])
        a = m.toList(rowmajor=0)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "Konstruktor ([4xvec4]) falsch")

    ######################################################################
    def testCmp(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        self.failUnless(M==N, "mat4==mat4 (1) falsch")
        self.failIf(M!=N, "mat4!=mat4 (1) falsch")

        M = mat4(2,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        self.failUnless(M!=N, "mat4!=mat4 (2) falsch")
        self.failIf(M==N, "mat4==mat4 (2) falsch")

    ######################################################################
    def testAdd(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        C = M+N
        self.failUnless(C==mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32),
                        "mat4 Addition falsch")

    ######################################################################
    def testSub(self):
        M = mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        C = M-N
        self.failUnless(C==N,
                        "mat4 Subtraktion falsch")

    ######################################################################
    def testMul(self):
        M = mat4(1,4,-2,3,5,7,2,0,3,-2,-1,1,3,4,-5,3)
        N = mat4(8,6,3,-4,-1,2,3,-7,3,0,-2,3,5,1,2,1)

        C = M*N
        self.failUnless(C==mat4(13,17,25,-35, 39,44,32,-63, 28,15,7,0, 20,29,37,-52),
                        "mat4 Multiplikation falsch")


        C = 2.0*M
        self.failUnless(C==mat4(2,8,-4,6,10,14,4,0,6,-4,-2,2,6,8,-10,6),
                        "mat4 Skalar-Multiplikation (links) falsch")

        C = M*2.0
        self.failUnless(C==mat4(2,8,-4,6,10,14,4,0,6,-4,-2,2,6,8,-10,6),
                        "mat4 Skalar-Multiplikation (rechts) falsch")

        v = vec4(2,4,-1,3)
        c = M*v
        self.assertEqual(c, vec4(29,36,2,36))

        c = v*M
        self.assertEqual(c, vec4(28,50,-10,14))

        v = vec3(2,4,-1)
        w = vec4(2,4,-1,1)
        c = M*v
        d = M*w
        e = vec3(d.x/d.w,d.y/d.w,d.z/d.w)
        self.assertEqual(c,e)

        c = v*M
        d = w*M
        e = vec3(d.x/d.w,d.y/d.w,d.z/d.w)
        self.assertEqual(c, e)

    ######################################################################
    def testDiv(self):
        M = mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        C = M/2.0
        self.failUnless(C==N,
                        "mat4 Division falsch")

    ######################################################################
    def testMod(self):
        M = mat4(2,4,6,1.4,-1.8,12,1,1,1.8,2.0,2.2,2.4,-2.6,1,3,2)

        self.failUnless(M%2.0==mat4(0,0,0,1.4,0.2,0,1,1,1.8,0,0.2,0.4,1.4,1,1,0),
                        "mat4 modulo (1) falsch")

        N = mat4(2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2)
        self.failUnless(M%N==mat4(0,0,0,1.4,0.2,0,1,1,1.8,0,0.2,0.4,1.4,1,1,0),
                        "mat4 modulo (2) falsch")
        

    ######################################################################
    def testNeg(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        C = -M
        self.failUnless(C==mat4(-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16),
                        "mat4 Neg falsch")

    ######################################################################
    def testPos(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        C = +M
        self.failUnless(C==mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
                        "mat4 Pos falsch")

    ######################################################################
    def testIAdd(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        M+=N
        self.failUnless(M==mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32),
                        "mat4 inline Addition falsch")

    ######################################################################
    def testISub(self):
        M = mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        M-=N
        self.failUnless(M==N,
                        "mat4 inline Subtraktion falsch")

    ######################################################################
    def testIMul(self):
        M = mat4(1,4,-2,3,5,7,2,0,3,-2,-1,1,3,4,-5,3)
        N = mat4(8,6,3,-4,-1,2,3,-7,3,0,-2,3,5,1,2,1)

        M*=N
        self.failUnless(M==mat4(13,17,25,-35, 39,44,32,-63, 28,15,7,0, 20,29,37,-52),
                        "mat4 inline Multiplikation falsch")


        M = mat4(1,4,-2,3,5,7,2,0,3,-2,-1,1,3,4,-5,3)
        M*=2.0
        self.failUnless(M==mat4(2,8,-4,6,10,14,4,0,6,-4,-2,2,6,8,-10,6),
                        "mat4 inline Skalar-Multiplikation falsch")


    ######################################################################
    def testIDiv(self):
        M = mat4(2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32)
        N = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        M/=2.0
        self.failUnless(M==N,
                        "mat4 inline Division falsch")

    ######################################################################
    def testIMod(self):
        M = mat4(2,4,6,1.4,-1.8,12,1,1,1.8,2.0,2.2,2.4,-2.6,1,3,2)
        
        M%=2.0
        self.failUnless(M==mat4(0,0,0,1.4,0.2,0,1,1,1.8,0,0.2,0.4,1.4,1,1,0),
                        "mat4 inline modulo (1) falsch")

        M = mat4(2,4,6,1.4,-1.8,12,1,1,1.8,2.0,2.2,2.4,-2.6,1,3,2)
        N = mat4(2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2)
        M%=N
        self.failUnless(M==mat4(0,0,0,1.4,0.2,0,1,1,1.8,0,0.2,0.4,1.4,1,1,0),
                        "mat4 inline modulo (2) falsch")


    ######################################################################
    def testGetItem(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        v=M[0]
        self.failUnless(v==vec4(1,5,9,13), "mat4[0] falsch")
        v=M[1]
        self.failUnless(v==vec4(2,6,10,14), "mat4[1] falsch")
        v=M[2]
        self.failUnless(v==vec4(3,7,11,15), "mat4[2] falsch")
        v=M[3]
        self.failUnless(v==vec4(4,8,12,16), "mat4[3] falsch")

        self.failUnless(M[0,0]==1, "mat4[i,j] falsch")
        self.failUnless(M[0,1]==2, "mat4[i,j] falsch")
        self.failUnless(M[0,2]==3, "mat4[i,j] falsch")
        self.failUnless(M[0,3]==4, "mat4[i,j] falsch")
        self.failUnless(M[1,0]==5, "mat4[i,j] falsch")
        self.failUnless(M[1,1]==6, "mat4[i,j] falsch")
        self.failUnless(M[1,2]==7, "mat4[i,j] falsch")
        self.failUnless(M[1,3]==8, "mat4[i,j] falsch")
        self.failUnless(M[2,0]==9, "mat4[i,j] falsch")
        self.failUnless(M[2,1]==10, "mat4[i,j] falsch")
        self.failUnless(M[2,2]==11, "mat4[i,j] falsch")
        self.failUnless(M[2,3]==12, "mat4[i,j] falsch")
        self.failUnless(M[3,0]==13, "mat4[i,j] falsch")
        self.failUnless(M[3,1]==14, "mat4[i,j] falsch")
        self.failUnless(M[3,2]==15, "mat4[i,j] falsch")
        self.failUnless(M[3,3]==16, "mat4[i,j] falsch")

    ######################################################################
    def testSetItem(self):
        M = mat4(0)
        M[0]=vec4(1,5,9,13)
        M[1]=vec4(2,6,10,14)
        M[2]=vec4(3,7,11,15)
        M[3]=vec4(4,8,12,16)
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        M = mat4(0)
        M[0]=(1,5,9,13)
        M[1]=(2,6,10,14)
        M[2]=(3,7,11,15)
        M[3]=(4,8,12,16)
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        M = mat4(0)
        M[0,0] = 1
        M[0,1] = 2
        M[0,2] = 3
        M[0,3] = 4
        M[1,0] = 5
        M[1,1] = 6
        M[1,2] = 7
        M[1,3] = 8
        M[2,0] = 9
        M[2,1] = 10
        M[2,2] = 11
        M[2,3] = 12
        M[3,0] = 13
        M[3,1] = 14
        M[3,2] = 15
        M[3,3] = 16

        self.failUnless(M==mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
                        "mat4 SetItem (2) falsch")

    ######################################################################
    def testGetRow(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        v=M.getRow(0)
        self.failUnless(v==vec4(1,2,3,4), "mat4.getRow(0) falsch")
        v=M.getRow(1)
        self.failUnless(v==vec4(5,6,7,8), "mat4.getRow(1) falsch")
        v=M.getRow(2)
        self.failUnless(v==vec4(9,10,11,12), "mat4.getRow(2) falsch")
        v=M.getRow(3)
        self.failUnless(v==vec4(13,14,15,16), "mat4.getRow(3) falsch")

        try:
            v=M.getRow(-1)
            v=M.getRow(4)
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
        M = mat4(0)
        M.setRow(0,vec4(1,2,3,4))
        M.setRow(1,vec4(5,6,7,8))
        M.setRow(2,vec4(9,10,11,12))
        M.setRow(3,vec4(13,14,15,16))
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        M = mat4(0)
        M.setRow(0,(1,2,3,4))
        M.setRow(1,(5,6,7,8))
        M.setRow(2,(9,10,11,12))
        M.setRow(3,(13,14,15,16))
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        try:
            M.setRow(-1,vec4(1,2,3,4))
            M.setRow(4,vec4(1,2,3,4))
            self.fail("setRow() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.setRow("2", vec4(1,2,3,4))
            self.fail("setRow(string) wirft keine Exception")
        except TypeError:
            pass

        try:
            v=M.setRow(0, vec3(1,2,3))
            self.fail("setRow(0,vec3) wirft keine Exception")
        except: #  ValueError:
            pass

    ######################################################################
    def testGetColumn(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        v=M.getColumn(0)
        self.failUnless(v==vec4(1,5,9,13), "mat4.getColumn(0) falsch")
        v=M.getColumn(1)
        self.failUnless(v==vec4(2,6,10,14), "mat4.getColumn(1) falsch")
        v=M.getColumn(2)
        self.failUnless(v==vec4(3,7,11,15), "mat4.getColumn(2) falsch")
        v=M.getColumn(3)
        self.failUnless(v==vec4(4,8,12,16), "mat4.getColumn(3) falsch")

        try:
            v=M.getColumn(-1)
            v=M.getColumn(4)
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
        M = mat4(0)
        M.setColumn(0,vec4(1,5,9,13))
        M.setColumn(1,vec4(2,6,10,14))
        M.setColumn(2,vec4(3,7,11,15))
        M.setColumn(3,vec4(4,8,12,16))
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        M = mat4(0)
        M.setColumn(0,(1,5,9,13))
        M.setColumn(1,(2,6,10,14))
        M.setColumn(2,(3,7,11,15))
        M.setColumn(3,(4,8,12,16))
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

        try:
            M.setColumn(-1,vec4(1,2,3,4))
            M.setColumn(4,vec4(1,2,3,4))
            self.fail("setColumn() mit falschem Index wirft keine Exception")
        except IndexError:
            pass

        try:
            v=M.setColumn("2", vec4(1,2,3,4))
            self.fail("setColumn(string) wirft keine Exception")
        except TypeError:
            pass

        try:
            v=M.setColumn(0, vec3(1,2,3))
            self.fail("setColumn(0,vec3) wirft keine Exception")
        except: # ValueError:
            pass

    ######################################################################
    def testDiag(self):
        M = mat4(1,2,3,4,
                 5,6,7,8,
                 9,10,11,12,
                 13,14,15,16)
        self.assertEqual(M.getDiag(), vec4(1,6,11,16))
        M.setDiag(vec4(-1,-6,-11,-16))
        self.assertEqual(M, mat4(-1,2,3,4,5,-6,7,8,9,10,-11,12,13,14,15,-16))
        M.setDiag(vec4(1,6,11,16))
        self.assertEqual(M, mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16))

    ######################################################################
    def testToList(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        a = M.toList()
        self.failUnless(a==[1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16],
                        "mat4.toList() falsch")

        a = M.toList(rowmajor=1)
        self.failUnless(a==[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
                        "mat4.toList() falsch")

    ######################################################################
    def testIdentity(self):
        M = mat4().identity()
        self.assertEqual(M, mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1))

        M = mat4.identity()
        self.assertEqual(M, mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1))

    ######################################################################
    def testTranspose(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)

        N = M.transpose()
        self.failUnless(N==mat4(1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16),
                        "mat4.transpose() falsch")

    ######################################################################
    def testDet(self):
        M = mat4().rotation(0.5, vec3(1,1,1))

        d = round(M.determinant(), 10)
        self.failUnless(d==1.0,
                        "mat4.determinant() (1) falsch")

        M[0] = -M[0]
        d = round(M.determinant(), 10)
        self.failUnless(d==-1.0,
                        "mat4.determinant() (2) falsch")

    ######################################################################
    def testInverse(self):
        M = mat4().rotation(0.5, vec3(1,1,1))
        N = mat4().rotation(-0.5, vec3(1,1,1))

        Mi = M.inverse()
        self.assertEqual(Mi, N)

        M = mat4(1,2,3,5,-1,2,3,1,-5,3,1,1,2,6,-2,2)
        Mi = M.inverse()
        self.assertEqual(M*Mi, mat4(1))

        M[1]=M[3]
        try:
            Mi = M.inverse()
            self.fail("mat4.inverse() wirft keine Exception")
        except ZeroDivisionError:
            pass

    ######################################################################
    def testTranslation(self):
        M = mat4().translation(vec3(1,2,3))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

        M = mat4.translation((1,2,3))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

    ######################################################################
    def testTranslate(self):
        M = mat4(1).translate(vec3(1,2,3))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

        M = mat4(1).translate((1,2,3))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

    ######################################################################
    def testScaling(self):
        M = mat4().scaling(vec3(2,3,-2))
        self.assertEqual(M, mat4(2,0,0,0, 0,3,0,0, 0,0,-2,0, 0,0,0,1))

        M = mat4.scaling((2,3,-2))
        self.assertEqual(M, mat4(2,0,0,0, 0,3,0,0, 0,0,-2,0, 0,0,0,1))

    ######################################################################
    def testScale(self):
        M = mat4(1).scale(vec3(2,3,-2))
        self.assertEqual(M, mat4(2,0,0,0, 0,3,0,0, 0,0,-2,0, 0,0,0,1))

        M = mat4(1).scale((2,3,-2))
        self.assertEqual(M, mat4(2,0,0,0, 0,3,0,0, 0,0,-2,0, 0,0,0,1))

    ######################################################################
    def testRotation(self):
        M = mat4().rotation(math.pi/2, vec3(0,0,1))
        self.assertEqual(M, mat4(0,-1,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,1))

        M = mat4.rotation(math.pi/2, (0,0,1))
        self.assertEqual(M, mat4(0,-1,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,1))

    ######################################################################
    def testRotate(self):
        M = mat4(1).rotate(math.pi/2, vec3(0,0,1))
        self.assertEqual(M, mat4(0,-1,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,1))

        M = mat4(1).rotate(math.pi/2, (0,0,1))
        self.assertEqual(M, mat4(0,-1,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,1))

    ######################################################################
    def testOrtho(self):
        M = mat4(1,2,3,4,5,6,7,8,9,10,11,12,0,0,0,1)

        C = M.ortho()
        b1 = C[0]
        b2 = C[1]
        b3 = C[2]
        a = vec3(b1*b2, b2*b3, b1*b3)
        self.failUnless(a==vec3(0,0,0),
                        "mat4 ortho falsch")

    ######################################################################
    def testFrustum(self):
        M = mat4().frustum(-1,1,-1,1,-1,1)
        self.assertEqual(M, mat4(-1,0,0,0, 0,-1,0,0, 0,0,0,1, 0,0,-1,0))

        M = mat4.frustum(-1,1,-1,1,-1,1)
        self.assertEqual(M, mat4(-1,0,0,0, 0,-1,0,0, 0,0,0,1, 0,0,-1,0))

    ######################################################################
    def testPerspective(self):
        eps = setEpsilon(1E-10)

        M = mat4().perspective(45, 1.333, -1, 1)
        self.assertEqual(M, mat4(1.81111295002,0,0,0, 0,2.41421356237,0,0, 0,0,0,1, 0,0,-1,0))

        M = mat4.perspective(45, 1.333, -1, 1)
        self.assertEqual(M, mat4(1.81111295002,0,0,0, 0,2.41421356237,0,0, 0,0,0,1, 0,0,-1,0))

        setEpsilon(eps)

    ######################################################################
    def testOrthographic(self):
        M = mat4().orthographic(-1,1,-1,1,-1,1)
        self.assertEqual(M, mat4(1,0,0,0, 0,1,0,0, 0,0,-1,0, 0,0,0,1))

        M = mat4.orthographic(-1,1,-1,1,-1,1)
        self.assertEqual(M, mat4(1,0,0,0, 0,1,0,0, 0,0,-1,0, 0,0,0,1))

    ######################################################################
    def testLookAt(self):
        M = mat4().lookAt(vec3(1,2,3), vec3(1,2,3+1), up=vec3(0,1,0))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

        M = mat4.lookAt((1,2,3), (1,2,3+1), (0,1,0))
        self.assertEqual(M, mat4(1,0,0,1, 0,1,0,2, 0,0,1,3, 0,0,0,1))

    ######################################################################
    def testDecompose(self):
        T = mat4().translation(vec3(1,2,3))
        S = mat4().scaling(vec3(2,3,2))
        R = mat4().rotation(0.5, vec3(1,0.2,1))
        M = T*R*S

        t,r,s = M.decompose()
        self.assertEqual(t, vec3(1,2,3))
        self.assertEqual(s, vec3(2,3,2))
        self.assertEqual(r, R)
        N=mat4(1).translate(t)
        N=N*R
        N=N.scale(s)
        self.assertEqual(N, M)

    ######################################################################
    def testGetSetMat3(self):
        m = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        self.assertEqual(m.getMat3(), mat3(1,2,3,5,6,7,9,10,11))

        n = mat3(-1,-2,-3,-4,-5,-6,-7,-8,-9)
        m.setMat3(n)
        self.assertEqual(m, mat4(-1,-2,-3,4,-4,-5,-6,8,-7,-8,-9,12,13,14,15,16))

    ######################################################################
    def testPickle(self):
        fname = "delme_pickle.dat"
        
        f = open(fname, "wb")
        m = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        pickle.dump(m, f)
        f.close()

        f = open(fname, "rb")
        n = pickle.load(f)
        f.close()

        os.remove(fname)

        self.assertEqual(m, n)

    ######################################################################
    def testCopy(self):
        """Test copying an internal mat4.
        """
        m = mat4.translation((1,2,3))
        m2 = copy.copy(m)
        self.assertEqual(m, m2)


######################################################################

setEpsilon(1E-12)

if __name__=="__main__":
    unittest.main()
