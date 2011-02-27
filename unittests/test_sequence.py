# Test the sequence module

import unittest
import os, os.path
import glob as globmod
from cgkit.sequence import *

class TestSeqString(unittest.TestCase):
    """Test the SeqString class.
    """
    
    def testComparison(self):
        """Test the comparison operator.
        """
        s1 = SeqString("spam004")
        s2 = SeqString("spam2")
        s3 = SeqString("spam4")
        self.assertEqual(1, cmp(s1, s2))
        self.assertEqual(-1, cmp(s2, s1))
        self.assertEqual(0, cmp(s1, s3))

        s1 = SeqString("spam004_018")
        s2 = SeqString("spam2_431")
        s3 = SeqString("spam4_18")
        self.assertEqual(1, cmp(s1, s2))
        self.assertEqual(-1, cmp(s2, s1))
        self.assertEqual(0, cmp(s1, s3))
        
        self.assertEqual("spam004_018", s1)
        self.assertEqual(True, "spam004_018"==s1)
        self.assertEqual(True, "spam04_18"==s1)
        self.assertEqual(True, "spam04_19"!=s1)
        
        s = SeqString("spam1_1")
        self.assertEqual(True, s=="spam1_1")
        s.replaceNum(0, "1")
        self.assertEqual(True, s=="spam1_1")
    
    def testMatch(self):
        """Test the match method.
        """
        s = SeqString("foo")
        self.assertEqual(True, s.match(SeqString("foo")))
        self.assertEqual(False, s.match(SeqString("Foo")))
        self.assertEqual(False, s.match(SeqString("bar")))
        self.assertEqual(False, s.match(SeqString("foo10")))
        self.assertEqual(False, s.match(SeqString("")))

        s = SeqString("spam1_bla05")
        self.assertEqual(True, s.match(SeqString("spam05_bla1000")))
        self.assertEqual(False, s.match(SeqString("spam05_bla1000x")))
        self.assertEqual(False, s.match(SeqString("spam_bla1000x")))
        self.assertEqual(False, s.match(SeqString("spam2_bla07"),-1))
        self.assertEqual(True, s.match(SeqString("spam1_bla07"),-1))
        self.assertEqual(False, s.match(SeqString("spam1_bla07"),0))
        self.assertEqual(False, s.match(SeqString("spam1_bla5"),0))
        self.assertEqual(True, s.match(SeqString("spam2_bla05"),0))
        self.assertEqual(False, s.match(SeqString("spam2_bla05"),3))
        self.assertEqual(True, s.match(SeqString("spam1_bla05"),3))
        self.assertEqual(False, s.match(SeqString("spam2_bla04"),-3))

        self.assertRaises(TypeError, lambda: s.match("spam2_bla04"))
        
    def testGroupRepr(self):
        """Test the groupRepr() method.
        """
        self.assertEqual("foo", SeqString("foo").groupRepr())
        self.assertEqual("foo*.tif", SeqString("foo100.tif").groupRepr())
        self.assertEqual("spam#_#", SeqString("spam3_0983").groupRepr("#"))
        self.assertEqual("spam_", SeqString("spam3_0983").groupRepr(""))

    def testNumCount(self):
        """Test the numCount() method.
        """
        self.assertEqual(1, SeqString("anim01").numCount())
        self.assertEqual(2, SeqString("anim2_01").numCount())
        self.assertEqual(0, SeqString("anim").numCount())
        
    def testGetNum(self):
        """Test the getNum() method.
        """
        self.assertEqual(17, SeqString("spam17_042_008").getNum(0))
        self.assertEqual(42, SeqString("spam17_042_008").getNum(1))
        self.assertEqual(8, SeqString("spam17_042_008").getNum(2))
        self.assertEqual(17, SeqString("spam17_042_008").getNum(-3))
        self.assertEqual(42, SeqString("spam17_042_008").getNum(-2))
        self.assertEqual(8, SeqString("spam17_042_008").getNum(-1))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042_008").getNum(3))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042_008").getNum(-4))
        self.assertRaises(IndexError, lambda: SeqString("spam").getNum(0))

    def testGetNumStr(self):
        """Test the getNumStr() method.
        """
        self.assertEqual("17", SeqString("spam17_042_008").getNumStr(0))
        self.assertEqual("042", SeqString("spam17_042_008").getNumStr(1))
        self.assertEqual("008", SeqString("spam17_042_008").getNumStr(2))
        self.assertEqual("17", SeqString("spam17_042_008").getNumStr(-3))
        self.assertEqual("042", SeqString("spam17_042_008").getNumStr(-2))
        self.assertEqual("008", SeqString("spam17_042_008").getNumStr(-1))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042_008").getNumStr(3))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042_008").getNumStr(-4))
        self.assertRaises(IndexError, lambda: SeqString("spam").getNumStr(0))
        
    def testGetNums(self):
        """Test the getNums() method.
        """
        self.assertEqual([17,42,8], SeqString("spam17_042_008").getNums())
        self.assertEqual([], SeqString("spam").getNums())
        
    def testSetNum(self):
        """Test the setNum() method.
        """
        s = SeqString("spam1_012")
        s.setNum(0, 2)
        self.assertEqual("spam2_012", str(s))
        s.setNum(1, 7)
        self.assertEqual("spam2_007", str(s))
        s.setNum(-1, 4)
        self.assertEqual("spam2_004", str(s))
        s.setNum(-2, 3)
        self.assertEqual("spam3_004", str(s))
        s.setNum(-1, 9876)
        self.assertEqual("spam3_9876", str(s))
        s.setNum(-1, 1)
        self.assertEqual("spam3_001", str(s))
        s.setNum(-1, -2)
        self.assertEqual("spam3_-02", str(s))
        self.assertEqual(-2, s.getNum(-1))
        s.setNum(-1, 7.6)
        self.assertEqual("spam3_007", str(s))
        s.setNum(0, 2, width=4)
        self.assertEqual("spam0002_007", str(s))
        
        self.assertRaises(IndexError, lambda: s.setNum(2, 5))
        self.assertRaises(IndexError, lambda: s.setNum(-3, 5))
        
    def testSetNums(self):
        """Test the setNums() method.
        """
        s = SeqString("spam1_012")
        s.setNums([3,4])
        self.assertEqual("spam3_004", str(s))
        s.setNums([2])
        self.assertEqual("spam2_004", str(s))
        
        self.assertRaises(IndexError, lambda: s.setNums([1,2,3]))

    def testGetNumWidth(self):
        """Test the getNumWidth() and getNumWidths() method.
        """
        self.assertEqual(2, SeqString("spam17_0042").getNumWidth(0))
        self.assertEqual(4, SeqString("spam17_0042").getNumWidth(1))
        self.assertEqual(2, SeqString("spam17_0042").getNumWidth(-2))
        self.assertEqual(4, SeqString("spam17_0042").getNumWidth(-1))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042").getNumWidth(2))
        self.assertRaises(IndexError, lambda: SeqString("spam17_042").getNumWidth(-3))
        self.assertRaises(IndexError, lambda: SeqString("spam").getNumWidth(0))

        self.assertEqual([2,4], SeqString("spam17_0042").getNumWidths())
        
    def testSetNumWidth(self):
        """Test the setNumWidth() and setNumWidths() method.
        """
        s = SeqString("spam1_004")
        s.setNumWidth(0, 2)
        self.assertEqual("spam01_004", str(s))
        s.setNumWidth(0, 0)
        self.assertEqual("spam1_004", str(s))
        s.setNumWidth(1, 4)
        self.assertEqual("spam1_0004", str(s))
        s.setNumWidth(-1, 1)
        self.assertEqual("spam1_4", str(s))
        s.setNumWidth(-2, 2)
        self.assertEqual("spam01_4", str(s))
        
        s.setNumWidths([3,4])
        self.assertEqual("spam001_0004", str(s))
        self.assertRaises(IndexError, lambda: s.setNumWidths([1,2,3]))
        
    def testDeleteNum(self):
        """Test the deleteNum() method.
        """
        s = SeqString("spam1_017_0003")
        s.deleteNum(1)
        self.assertEqual("spam1__0003", str(s))
        self.assertEqual(2, s.numCount())
        s = SeqString("spam1_017_0003")
        s.deleteNum(-3)
        self.assertEqual("spam_017_0003", str(s))
        self.assertRaises(IndexError, lambda: s.deleteNum(3))
        self.assertRaises(IndexError, lambda: s.deleteNum(-4))

    def testReplaceNum(self):
        """Test the replaceNum() method.
        """
        s = SeqString("spam1_017_0003")
        s.replaceNum(2, "foo")
        self.assertEqual("spam1_017_foo", str(s))
        self.assertEqual(2, s.numCount())
        s.replaceNum(-2, "*")
        self.assertEqual("spam*_017_foo", str(s))
        self.assertEqual(1, s.numCount())
        s.replaceNum(0, "#")
        self.assertEqual("spam*_#_foo", str(s))
        self.assertEqual(0, s.numCount())
        self.assertRaises(IndexError, lambda: s.deleteNum(0))

        s = SeqString("03")
        s.replaceNum(0, "foo")
        self.assertEqual("foo", str(s))
        self.assertEqual(0, s.numCount())

        s = SeqString("03")
        s.replaceNum(0, "")
        self.assertEqual("", str(s))
        self.assertEqual(0, s.numCount())

    def testReplaceStr(self):
        """Test the replaceStr() method.
        """
        s = SeqString("spam1_017_0003")
        s.replaceStr(0, "foo")
        self.assertEqual("foo1_017_0003", str(s))
        self.assertEqual(3, s.numCount())
        s.replaceStr(2, "--")
        self.assertEqual("foo1_017--0003", str(s))
        s.replaceStr(3, ".tif")
        self.assertEqual("foo1_017--0003.tif", str(s))

        s = SeqString("spam1_017_0003")
        s.replaceStr(-4, "foo")
        self.assertEqual("foo1_017_0003", str(s))
        self.assertEqual(3, s.numCount())
        s.replaceStr(-2, "--")
        self.assertEqual("foo1_017--0003", str(s))
        s.replaceStr(-1, ".tif")
        self.assertEqual("foo1_017--0003.tif", str(s))

        self.assertRaises(IndexError, lambda: s.replaceStr(4, "#"))

        s = SeqString("017_0003")
        self.assertEqual(2, s.numCount())
        s.replaceStr(0, "spam")
        self.assertEqual("spam017_0003", str(s))
        self.assertEqual(2, s.numCount())

        s = SeqString("2")
        s.replaceStr(0, "spam")
        s.replaceStr(1, "eggs")
        self.assertEqual("spam2eggs", str(s))

        s = SeqString("")
        s.replaceStr(0, "spam")
        self.assertEqual("spam", str(s))
        self.assertRaises(IndexError, lambda: s.replaceStr(1, "foo"))


class TestRange(unittest.TestCase):
    """Test the Range object.
    """
    
    def testRange(self):
        r = Range()
        self.assertEqual(0, len(r))
        self.assertEqual([], list(r))
        self.assertEqual("", str(r))
        self.assertEqual(False, r.isInfinite())
        
        r = Range("2-10")
        self.assertEqual(9, len(r))
        self.assertEqual([2,3,4,5,6,7,8,9,10], list(r))
        self.assertEqual("2-10", str(r))
        self.assertEqual(False, r.isInfinite())

        r = Range("1,9-9, 3 ,5-7")
        self.assertEqual(6, len(r))
        self.assertEqual([1,3,5,6,7,9], list(r))
        self.assertEqual("1-5x2,6-7,9", str(r))
        for i in range(15):
            self.assertEqual(i in [1,3,5,6,7,9], i in r)

        r = Range("2-10x2")
        self.assertEqual(5, len(r))
        self.assertEqual([2,4,6,8,10], list(r))
        self.assertEqual("2-10x2", str(r))
        self.assertEqual(False, r.isInfinite())

        r = Range("6-3")
        self.assertEqual(0, len(r))
        self.assertEqual([], list(r))
        self.assertEqual("", str(r))

        r = Range("6-")
        self.assertEqual(True, r.isInfinite())
        self.assertRaises(ValueError, lambda: len(r))
        it = iter(r)
        self.assertEqual(6, it.next())
        self.assertEqual(7, it.next())
        self.assertEqual(8, it.next())
        self.assertEqual("6-", str(r))
        self.assertEqual(False, 5 in r)
        self.assertEqual(True, 6 in r)
        self.assertEqual(True, 9867324 in r)
        for i in range(15):
            self.assertEqual(i>5, i in r)
        
        r.setRange("1-3")
        self.assertEqual(False, r.isInfinite())
        self.assertEqual(3, len(r))
        self.assertEqual([1,2,3], list(r))
        
        self.assertRaises(ValueError, lambda: r.setRange("spam"))
        
    def testNormalization(self):
        """Test the normalization.
        """
        self.assertEqual("2-10", str(Range("2-8x2,3-10")))
        self.assertEqual("1-10", str(Range("1-5,6-10")))
        self.assertEqual("1-5x2,6-10", str(Range("1-9x2,5-10")))
        self.assertEqual("1-9x2", str(Range("1-9x2,5")))
        self.assertEqual("1-11x2", str(Range("1-9x2,5-11x2")))
        self.assertEqual("1-5x2,6,7,8-10,12", str(Range("1-9x2,6-12x2")))
        self.assertEqual("1-4,6", str(Range("1-4,3-6x3")))
        self.assertEqual("1-7x3,8-12", str(Range("1-7x3,7-12")))
        self.assertEqual("4,7-12", str(Range("4-7x3,7-12")))
        self.assertEqual("1-3x2", str(Range("1,3")))
        self.assertEqual("10-16x2,17,18-x2", str(Range("10-x2,17")))
        self.assertEqual("10-20x2,21-", str(Range("10-x2,21-")))
        
    def testComparison(self):
        """Test comparing range objects.
        """
        r1 = Range("1-5,8")
        r2 = Range("8,1-5")
        r3 = Range("1-5,7")
        r4 = Range("8,1-4,5")

        self.assertEqual(True, r1==r2)
        self.assertEqual(False, r1==r3)
        self.assertEqual(True, r1==r4)
        self.assertEqual(False, r1!=r2)
        self.assertEqual(True, r1!=r3)
        self.assertEqual(False, r1!=r4)
        self.assertEqual(False, r1==None)
        self.assertEqual(True, r1!=None)
        self.assertEqual(False, r1==5)
        self.assertEqual(True, r1!=5)


class TestSequence(unittest.TestCase):
    """Test the Sequence class.
    """
    
    def testSequence(self):
        seq = Sequence()
        self.assertEqual(0, len(seq))
        self.assertEqual([], list(seq))
        self.assertEqual(None, seq.sequenceNumberIndex())
        
        seq.append("spam1")
        self.assertEqual(1, len(seq))
        self.assertEqual(["spam1"], list(seq))
        self.assertEqual(True, isinstance(seq[0], SeqString))
        self.assertEqual(("spam1", []), seq.sequenceName())

        seq.append("spam3")
        self.assertEqual(2, len(seq))
        self.assertEqual(["spam1", "spam3"], list(seq))
        self.assertEqual(("spam@", ["1,3"]), seq.sequenceName())

        seq.append("spam2")
        self.assertEqual(3, len(seq))
        self.assertEqual(["spam1", "spam3", "spam2"], list(seq))
        self.assertEqual(("spam@", ["1-3"]), seq.sequenceName())
        
        seq.append("spam3")
        self.assertEqual(4, len(seq))
        self.assertEqual(["spam1", "spam3", "spam2", "spam3"], list(seq))
        self.assertEqual(["spam1", "spam3", "spam2", "spam3"], list(seq.iterNames()))
        self.assertEqual(["spam1", "spam3", "spam2", "spam3"], list(seq.iterObjects()))
        self.assertEqual(("spam@", ["1-3"]), seq.sequenceName())

        self.assertEqual(SeqString("spam1"), seq[0])
        self.assertEqual(SeqString("spam3"), seq[1])
        self.assertEqual(SeqString("spam2"), seq[2])
        self.assertEqual(SeqString("spam3"), seq[3])
        self.assertRaises(IndexError, lambda: seq[4])
        self.assertEqual(SeqString("spam3"), seq[-1])
        self.assertEqual(SeqString("spam2"), seq[-2])
        self.assertEqual(SeqString("spam3"), seq[-3])
        self.assertEqual(SeqString("spam1"), seq[-4])
        self.assertRaises(IndexError, lambda: seq[-5])

        self.assertEqual([Range("1-3")], seq.ranges())
        self.assertEqual(0, seq.sequenceNumberIndex())
        
        self.assertRaises(ValueError, lambda: seq.append("spam4", obj=4))

        self.assertRaises(ValueError, lambda: seq.append("spam3_1"))

        self.assertEqual(True, seq.match("spam02"))
        self.assertEqual(False, seq.match("foo02"))
    
    def testMultiSequence(self):
        """Test a sequence that has more than 1 number in its name.
        """
        seq = Sequence()
        seq.append("clip1_02")
        self.assertEqual(1, len(seq))
        self.assertEqual(["clip1_02"], list(seq))
        self.assertEqual(("clip1_02", []), seq.sequenceName())
        self.assertEqual(1, seq.sequenceNumberIndex())

        seq.append("clip1_03")
        self.assertEqual(("clip1_@@", ["2-3"]), seq.sequenceName())
        self.assertEqual(1, seq.sequenceNumberIndex())

        seq = Sequence()
        seq.append("clip1_02")
        seq.append("clip2_02")
        self.assertEqual(("clip@_02", ["1-2"]), seq.sequenceName())
        self.assertEqual(0, seq.sequenceNumberIndex())

    def testSequenceObjects(self):
        """Test inserting objects instead of just names.
        """
        seq = Sequence()
        seq.append("spam1", obj=1)
        seq.append("spam2", obj=2)
        self.assertRaises(ValueError, lambda: seq.append("spam3"))
        self.assertEqual([1,2], list(seq))
        self.assertEqual(["spam1", "spam2"], list(seq.iterNames()))
        self.assertEqual([1,2], list(seq.iterObjects()))


class TestBuildSequences(unittest.TestCase):
    """Test the buildSequences() function.
    """
    
    def testBuildSequences(self):
        seqs = buildSequences([])
        self.assertEqual([], seqs)

        seqs = buildSequences(["spam1", "spam3", "spam2"])
        self.assertEqual(1, len(seqs))
        self.assertEqual(["spam1", "spam2", "spam3"], list(seqs[0]))

        seqs = buildSequences(["clip1_1", "clip2_1", "clip1_2", "clip2_2"])
        self.assertEqual(1, len(seqs))
        self.assertEqual(["clip1_1", "clip1_2", "clip2_1", "clip2_2"], list(seqs[0]))

        seqs = buildSequences(["clip1_1", "clip2_1", "clip1_2", "clip2_2"], numPos=-1)
        self.assertEqual(2, len(seqs))
        self.assertEqual(["clip1_1", "clip1_2"], list(seqs[0]))
        self.assertEqual(["clip2_1", "clip2_2"], list(seqs[1]))

        seqs = buildSequences([1,3,2], nameFunc=lambda obj: "obj%d"%obj)
        self.assertEqual([1,2,3], list(seqs[0].iterObjects()))
        self.assertEqual(["obj1", "obj2", "obj3"], list(seqs[0].iterNames()))

        seqs = buildSequences(["dir1/spam1", "dir2/spam1"])
        self.assertEqual(1, len(seqs))
        self.assertEqual(["dir1/spam1", "dir2/spam1"], list(seqs[0]))

        seqs = buildSequences(["dir1/spam1", "dir2/spam1"], assumeFiles=True)
        self.assertEqual(2, len(seqs))
        self.assertEqual(["dir1/spam1"], list(seqs[0]))
        self.assertEqual(["dir2/spam1"], list(seqs[1]))

class TestSeqTemplate(unittest.TestCase):
    """Test the SeqTemplate class.
    """
    
    def testSeqTemplate(self):
        
        t = SeqTemplate("foo#.tif")
        self.assertEqual("foo0007.tif", t([7]))
        self.assertEqual("foo0008.tif", t.substitute([8]))
        self.assertEqual([0], t.expressionIndices(2))

        t = SeqTemplate("foo#_@_@@.tif")
        self.assertEqual("foo0001_2_03.tif", t([1,2,3]))
        self.assertEqual([0,1,2], t.expressionIndices(3))

        t = SeqTemplate("foo#[3]_@[-1]_@@.tif")
        self.assertEqual("foo0003_3_03.tif", t([1,2,3]))
        self.assertEqual([2,2,2], t.expressionIndices(3))

        t = SeqTemplate("foo{2*#[3]+2}.tif")
        self.assertEqual("foo0008.tif", t([1,2,3]))
        self.assertEqual([2], t.expressionIndices(3))


class TestOutputNameGenerator(unittest.TestCase):
    """Test the OutputNameGenerator class.
    """
    
    def testGenerator(self):
        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo")
        self.assertEqual([("spam1_1.tif", "foo0001.tif"),
                          ("spam1_2.tif", "foo0002.tif"),
                          ("spam1_5.tif", "foo0005.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo@@_#.tif")
        self.assertEqual([("spam1_1.tif", "foo01_0001.tif"),
                          ("spam1_2.tif", "foo01_0002.tif"),
                          ("spam1_5.tif", "foo01_0005.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo_#[2]_{@[1]+2}.tif")
        self.assertEqual([("spam1_1.tif", "foo_0001_3.tif"),
                          ("spam1_2.tif", "foo_0002_3.tif"),
                          ("spam1_5.tif", "foo_0005_3.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo@@@.tif")
        self.assertEqual([("spam1_1.tif", "foo001.tif"),
                          ("spam1_2.tif", "foo002.tif"),
                          ("spam1_5.tif", "foo005.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam2_5.tif", "spam2_6.tif"])
        ong = OutputNameGenerator(seqs, "foo@_@.tif")
        self.assertEqual([("spam1_1.tif", "foo1_1.tif"),
                          ("spam1_2.tif", "foo1_2.tif"),
                          ("spam2_5.tif", "foo2_5.tif"),
                          ("spam2_6.tif", "foo2_6.tif")], list(ong))

        seqs = buildSequences(["spam2_1.tif", "spam3_1.tif", "spam4_1.tif", "spam5_1.tif"])
        ong = OutputNameGenerator(seqs, "foo@_@.tif", dstRange=Range("5-"))
        self.assertEqual([("spam2_1.tif", "foo5_1.tif"),
                          ("spam3_1.tif", "foo6_1.tif"),
                          ("spam4_1.tif", "foo7_1.tif"),
                          ("spam5_1.tif", "foo8_1.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam2_5.tif", "spam2_6.tif"])
        ong = OutputNameGenerator(seqs, "foo#", dstRange=Range("2-"))
        self.assertEqual([("spam1_1.tif", "foo0002.tif"),
                          ("spam1_2.tif", "foo0003.tif"),
                          ("spam2_5.tif", "foo0004.tif"),
                          ("spam2_6.tif", "foo0005.tif")], list(ong))

    def testSrcRanges(self):
        """Test the srcRanges argument.
        """
        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo", srcRanges=[Range("2,5")])
        self.assertEqual([("spam1_2.tif", "foo0002.tif"),
                          ("spam1_5.tif", "foo0005.tif")], list(ong))
        
    def testKeepExt(self):
        """Test the keepExt flag.
        """
        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo#.png", keepExt=False)
        self.assertEqual([("spam1_1.tif", "foo0001.png"),
                          ("spam1_2.tif", "foo0002.png"),
                          ("spam1_5.tif", "foo0005.png")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo#.png", keepExt=True)
        self.assertEqual([("spam1_1.tif", "foo0001.png.tif"),
                          ("spam1_2.tif", "foo0002.png.tif"),
                          ("spam1_5.tif", "foo0005.png.tif")], list(ong))

    def testEnforceDstRange(self):
        """Test the enforceDstRange and repeatSrc flags.
        """
        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo#", dstRange=Range("1-5"))
        self.assertEqual([("spam1_1.tif", "foo0001.tif"),
                          ("spam1_2.tif", "foo0002.tif"),
                          ("spam1_5.tif", "foo0003.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo#", dstRange=Range("1-7"), enforceDstRange=True)
        self.assertEqual([("spam1_1.tif", "foo0001.tif"),
                          ("spam1_2.tif", "foo0002.tif"),
                          ("spam1_5.tif", "foo0003.tif"),
                          ("spam1_1.tif", "foo0004.tif"),
                          ("spam1_2.tif", "foo0005.tif"),
                          ("spam1_5.tif", "foo0006.tif"),
                          ("spam1_1.tif", "foo0007.tif")], list(ong))

        seqs = buildSequences(["spam1_1.tif", "spam1_2.tif", "spam1_5.tif"])
        ong = OutputNameGenerator(seqs, "foo#", dstRange=Range("1-6"), enforceDstRange=True, repeatSrc=False)
        self.assertEqual([("spam1_1.tif", "foo0001.tif"),
                          ("spam1_2.tif", "foo0002.tif"),
                          ("spam1_5.tif", "foo0003.tif"),
                          ("spam1_5.tif", "foo0004.tif"),
                          ("spam1_5.tif", "foo0005.tif"),
                          ("spam1_5.tif", "foo0006.tif")], list(ong))


class TestSeqUtils(unittest.TestCase):
    """Test the sequence utilities.
    """
    
    def setUp(self):
        for fileName in globmod.glob("tmp/*.txt"):
            os.remove(fileName)
        for fileName in globmod.glob("tmp/*.ext"):
            os.remove(fileName)
        
    def testSeqCp1(self):
        """Test basic seqcp"""
        self.createSequence("tmp/spam#.txt", Range("1-5"))
        os.system("python ../utilities/seqcp.py tmp/spam tmp/foo")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("1-5"))
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("1-5"))
        
    def testSeqCp2(self):
        """Test seqcp -s"""
        self.createSequence("tmp/spam#.txt", Range("1-5"))
        os.system("python ../utilities/seqcp.py tmp/spam tmp/foo -s2-3")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("2-3"))
        if os.path.exists("tmp/foo0001.txt"):
            self.fail("foo0001.txt exists!")
        if os.path.exists("tmp/foo0004.txt"):
            self.fail("foo0004.txt exists!")
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("1-5"))

    def testSeqCp3(self):
        """Test seqcp of discontinuous sequence"""
        self.createSequence("tmp/spam#.txt", Range("1-2,5"))
        os.system("python ../utilities/seqcp.py tmp/spam tmp/foo")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("1,2,5"))
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("1,2,5"))

    def testSeqCp4(self):
        """Test seqcp -d"""
        self.createSequence("tmp/spam#.txt", Range("1-2,5"))
        os.system("python ../utilities/seqcp.py tmp/spam tmp/foo -d2-")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("2-4"), Range("1,2,5"))
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("1,2,5"))


    def testSeqMv1(self):
        """Test basic seqmv"""
        self.createSequence("tmp/spam#.txt", Range("1-5"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/foo")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("1-5"))
        
    def testSeqMv2(self):
        """Test seqmv -s"""
        self.createSequence("tmp/spam#.txt", Range("1-5"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/foo -s2-3")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("2-3"))
        if os.path.exists("tmp/foo0001.txt"):
            self.fail("foo0001.txt exists!")
        if os.path.exists("tmp/foo0004.txt"):
            self.fail("foo0004.txt exists!")

    def testSeqMv3(self):
        """Test seqmv of discontinuous sequence"""
        self.createSequence("tmp/spam#.txt", Range("1-2,5"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/foo")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("1,2,5"))

    def testSeqMv4(self):
        """Test seqmv -d"""
        self.createSequence("tmp/spam#.txt", Range("1-2,5"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/foo -d2-")
        self.assertFiles("tmp/foo#.txt", "spam#.txt", Range("2-4"), Range("1,2,5"))

    def testSeqMv5(self):
        """Test seqmv of overlapping src/dst ranges (move backward)"""
        self.createSequence("tmp/spam#.txt", Range("2-8"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/spam -d1-")
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("1-7"), Range("2-8"))

    def testSeqMv6(self):
        """Test seqmv of overlapping src/dst ranges (move forward)"""
        self.createSequence("tmp/spam#.txt", Range("2-8"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/spam -d3-")
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("3-9"), Range("2-8"))

    def testSeqMv7(self):
        """Test seqmv of overlapping src/dst ranges (needs tmp sequence)"""
        self.createSequence("tmp/spam#.txt", Range("2-10x2"))
        os.system("python ../utilities/seqmv.py tmp/spam tmp/spam -d4-")
        self.assertFiles("tmp/spam#.txt", "spam#.txt", Range("4-8"), Range("2-10x2"))

    def testSeqMv8(self):
        """Test the -e option (to change the extension)."""
        self.createSequence("tmp/spam#.ext", Range("1-3"))
        os.system('python ../utilities/seqmv.py "tmp/spam*.ext" tmp/foo@@.txt -e')
        self.assertFiles("tmp/foo@@.txt", "spam#.ext", Range("1-3"))
        
        
    def assertFiles(self, namePattern, origPattern, rng, origRng=None):
        if origRng is None:
            origRng = rng
            
        srcTmpl = SeqTemplate(namePattern)
        origTmpl = SeqTemplate(origPattern)
        for i,j in zip(rng, origRng):
            self.assertFile(srcTmpl([i]), origTmpl([j]))
        
    def assertFile(self, name, origName):
        """Assert that a file exists and originated from a particular file.
        
        name: File name to check.
        origName: The (base) name of the original file.
        """
        if not os.path.exists(name):
            self.fail("File %s does not exist"%name)
        f = open(name, "rt")
        name = f.read()
        f.close()
        self.assertEqual(origName, name)
    
    def createSequence(self, pattern, rng):
        """Create a file sequence.
        
        pattern is a string containing the name pattern (must contain one
        number substitution pattern). rng is a Range object containing the
        range of frames that should be generated.
        """
        tmpl = SeqTemplate(pattern)
        for i in rng:
            name = tmpl([i])
            f = open(name, "wt")
            f.write(os.path.basename(name))
            f.close()
            

######################################################################

if __name__=="__main__":
    unittest.main()
