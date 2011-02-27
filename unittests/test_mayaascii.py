# Test the mayaascii module

import unittest
from cgkit import mayaascii

class TestReader(mayaascii.MAReader):
    def onSetAttr(self, attr, vals, opts):
#        print attr,vals,opts
        a = mayaascii.Attribute(attr,vals,opts)
        #print "  ",a.getValue()

class TestMayaAscii(unittest.TestCase):

    def testDefaultMAReader(self):
        rd = TestReader()
        rd.read("data/objects.ma")
        
        rd = mayaascii.DefaultMAReader()
        rd.read("data/objects.ma")
        t = rd.nodelist[0]
#        print t.getAttrValue("t", "t", "double3")

    def testMELCommands(self):
        rd = mayaascii.DefaultMAReader()
        rd.read("data/testmel.ma")

    def testMAReader_findString(self):
        """Test the MAReader.findString() method.
        """

        rd = mayaascii.MAReader()
        self.assertEqual(rd.findString(''), (None, None))
        self.assertEqual(rd.findString('Test string'), (None, None))
        self.assertEqual(rd.findString('a="foo"'), (2, 6))
        self.assertEqual(rd.findString('a="foo'), (2, None))
        self.assertEqual(rd.findString(r'a="foo\"'), (2, None))
        self.assertEqual(rd.findString(r'a="foo\"" - "spam"'), (2, 8))
        self.assertEqual(rd.findString(r'a="foo\"spam\""'), (2, 14))

    def testMAReader_splitCommand(self):
        """Test the MAReader.splitCommand() method.
        """

        rd = mayaascii.MAReader()
        self.assertEqual(rd.splitCommand('setAttr -k off ".v";'), (["setAttr", "-k", "off", '".v"'],19))
        self.assertEqual(rd.splitCommand('setAttr -k off ".v"'), (["setAttr", "-k", "off", '".v"'],-1))
        self.assertEqual(rd.splitCommand('select -ne :defaultShaderList1;'), (["select", "-ne", ":defaultShaderList1"], 30))
        self.assertEqual(rd.splitCommand('select -ne :defaultShaderList1;setAttr -k off ".v"'), (["select", "-ne", ":defaultShaderList1"], 30))
        self.assertEqual(rd.splitCommand('select -ne :defaultShaderList1;setAttr -k off ".v'), (["select", "-ne", ":defaultShaderList1"], 30))
        self.assertEqual(rd.splitCommand('setAttr -k off ".v";select -ne :defaultShaderList1;'), (["setAttr", "-k", "off", '".v"'],19))
        self.assertEqual(rd.splitCommand('setAttr "-k off" foo ".v";select -ne :defaultShaderList1;'), (["setAttr", '"-k off"', "foo", '".v"'],25))
        self.assertEqual(rd.splitCommand('setAttr "foo spam"'), (["setAttr", '"foo spam"'],-1))
        self.assertEqual(rd.splitCommand('setAttr "foo spam'), (["setAttr", '"foo spam"'],-1))

    def testMAReader_getOpt(self):
        """Test the MAReader.splitCommand() method.
        """

        rd = mayaascii.MAReader()
        args,n = rd.splitCommand('setAttr -k off ".v"')
        self.assertEqual(rd.getOpt(args[1:], rd.setAttr_opt_def, rd.setAttr_name_dict), (['".v"'], dict(keyable=["off"])))

        args,n = rd.splitCommand('setAttr "-k" off ".v"')
        self.assertEqual(rd.getOpt(args[1:], rd.setAttr_opt_def, rd.setAttr_name_dict), (['".v"'], dict(keyable=["off"])))

        args,n = rd.splitCommand('setAttr ".cuvs" -type "string" "map1";')
        self.assertEqual(rd.getOpt(args[1:], rd.setAttr_opt_def, rd.setAttr_name_dict), (['".cuvs"', '"map1"'], dict(type=["string"])))

        self.assertEqual(rd.getOpt(["-5"], rd.setAttr_opt_def, rd.setAttr_name_dict), (["-5"], {}))


######################################################################

if __name__=="__main__":
    unittest.main()
