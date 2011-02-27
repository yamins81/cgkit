# Test the slparams module

import unittest
import os, os.path
from cgkit import slparams
from cgkit.cgtypes import *

class TestSlparams(unittest.TestCase):
    
    def testSlparams(self):
        """Test the slparams.slparams() function.
        """
        res = slparams.slparams("data/testshader.sl")
        self.assertTestShaderParams(res)
        
    def testCompiledShader(self):
        """Test reading params from compiled shaders.
        """
        renderers = [("3Delight", "shaderdl", "sdl"),
                     #("Aqsis", "aqsl", "slx"),
                     #("Pixie", "sdrc", "sdr"),
                     #("PRMan", "shader -C", "slo")
                     ]
        for data in renderers:
            renderer,shaderCompiler,shaderExt = data
            slcName = "tmp/testshader.%s"%shaderExt
            if os.path.exists(slcName):
                os.remove(slcName)
            cmd = "%s ../data/testshader.sl"%(shaderCompiler)
            os.chdir("tmp")
            res = os.system(cmd)
            os.chdir("..")
            if res!=0:
                print "WARNING: Skipping slparams test for %s. Shading compiler (%s) is not available."%(renderer, shaderCompiler)
                continue
            res = slparams.slparams(slcName)
            self.assertTestShaderParams(res)
            
            # Check the meta data (renderer-specific)
            if renderer=="3Delight":
                meta = res[0].meta
                self.assertEqual(meta, {"MyData": "MyValue"})
        
    def assertTestShaderParams(self, shaderInfo):
        """Check the result from slparams().
        """
        self.assertEqual(len(shaderInfo), 1)
        type,name,params = shaderInfo[0]
        
        self.assertEqual(type, "surface")
        self.assertEqual(name, "testshader")
        self.assertEqual(len(params), 10)
        self.assertEqual(params[0][:6], ('', 'uniform', 'float', None, 'Ka', None))
        self.assertEqual(1.0, slparams.convertdefault(params[0]))
        self.assertEqual(params[1][:6], ('', 'varying', 'vector', None, 'norm', "current"))
        self.assertEqual(vec3(0), slparams.convertdefault(params[1]))
        self.assertEqual(params[2][:6], ('', 'uniform', 'float', 2, 'uv', None))
        self.assertEqual([1.0, 2.0], slparams.convertdefault(params[2]))
        self.assertEqual(params[3][:6], ('output', 'uniform', 'point', None, 'out', "world"))
        self.assertEqual(vec3(0), slparams.convertdefault(params[3]))
        # The following test is disabled because it fails for 3Delight
#        self.assertEqual(params[4][:6], ('', 'uniform', 'point', 3, 'pnts', ["object", "world", "camera"]))
        self.assertEqual([vec3(1,2,3), vec3(4,5,6), vec3(7,8,9)], slparams.convertdefault(params[4]))
        self.assertEqual(params[5][:6], ('', 'uniform', 'color', None, 'col', "rgb"))
        self.assertEqual(vec3(1,1,1), slparams.convertdefault(params[5]))
        self.assertEqual(params[6][:6], ('', 'uniform', 'matrix', None, 'M', "current"))
        self.assertEqual(mat4(1), slparams.convertdefault(params[6]))
        self.assertEqual(params[7][:6], ('', 'uniform', 'matrix', None, 'T', "current"))
        self.assertEqual(mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16), slparams.convertdefault(params[7]))
        self.assertEqual(params[8][:6], ('', 'uniform', 'float', None, 'Kd', None))
        self.assertEqual(0.5, slparams.convertdefault(params[8]))
        self.assertEqual(params[9][:6], ('', 'uniform', 'float', 1, 'singleArr', None))
        self.assertEqual([0.5], slparams.convertdefault(params[9]))
        

######################################################################

if __name__=="__main__":
    unittest.main()
