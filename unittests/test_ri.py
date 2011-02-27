# Test the ri module

import os, os.path, shutil
import unittest
import cgkit.ri
import cgkit.cri
import ritest

class TestRi(unittest.TestCase):
    
    def setUp(self):
        tmpDir = os.path.join("tmp")
        if not os.path.exists("tmp"):
            os.mkdir("tmp")
        
#    def tearDown(self):
#        shutil.rmtree("tmp", ignore_errors=True)
    
    def testRiModule(self):
        """Check the ri module."""

        if os.path.exists("tmp/riout.rib"):
            os.remove("tmp/riout.rib")
        if os.path.exists("tmp/include.rib"):
            os.remove("tmp/include.rib")

        ritest.main(cgkit.ri, "tmp/riout.rib", archiveName="tmp/include.rib")
        
        f = file("tmp/riout_norm.rib", "wt")
        ritest.normalizeRIB(file("tmp/riout.rib", "rt"), f)
        f.close()

        f = file("tmp/include_norm.rib", "wt")
        ritest.normalizeRIB(file("tmp/include.rib", "rt"), f)
        f.close()
        
        self.assertFiles("tmp/riout_norm.rib", "data/riout_ref.rib")
        self.assertFiles("tmp/include_norm.rib", "data/include_ref.rib")
        
    def testCRiModule(self):
        """Check the cri module."""
        print ("cri test is disabled")
        return

        if os.path.exists("tmp/criout.rib"):
            os.remove("tmp/criout.rib")
        if os.path.exists("tmp/cinclude.rib"):
            os.remove("tmp/cinclude.rib")

#        ri = cgkit.cri.loadRI(None)
        ri = cgkit.cri.loadRI("3delight")
#        ri = cgkit.cri.loadRI("ri")
#        ri = cgkit.cri.loadRI("aqsislib")
#        ri = cgkit.cri.loadRI("ri2rib")
#        ri = cgkit.cri.loadRI("prman")
#        ri.RiProcFree = None

        ritest.main(ri, "tmp/criout.rib", archiveName="tmp/cinclude.rib")
        
        f = file("tmp/criout_norm.rib", "wt")
        ritest.normalizeRIB(file("tmp/criout.rib", "rt"), f)
        f.close()

        f = file("tmp/cinclude_norm.rib", "wt")
        ritest.normalizeRIB(file("tmp/cinclude.rib", "rt"), f)
        f.close()
        
        self.assertFiles("tmp/criout_norm.rib", "data/riout_ref.rib")
        self.assertFiles("tmp/cinclude_norm.rib", "data/cinclude_ref.rib")
        
        
    def assertFiles(self, fileName, refFileName):
        lines = file(fileName, "rt").readlines()
        reflines = file(refFileName, "rt").readlines()
        linenr = 0
        for line,refline in zip(lines, reflines):
            line = line.rstrip()
            refline = refline.rstrip()
            linenr += 1
            self.assertEqual(line, refline, "Mismatch in line %d: '%s' != '%s'"%(linenr, line.strip()[:30], refline.strip()[:30]))
            
        if len(lines)<len(reflines):
            self.fail('"%s" has too few lines'%fileName)
        elif len(lines)>len(reflines):
            self.fail('"%s" has too many lines'%fileName)
                

######################################################################

if __name__=="__main__":
    unittest.main()
