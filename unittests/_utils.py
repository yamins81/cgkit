# Utility functions that are used in more than just one test

from cgkit.geomobject import *

############### GeomObject tests: ##############

# createVars
def createVars(geom):
    """Create a variable for each storage class.
    """
    
    geom.newVariable("constant", CONSTANT, INT)
    geom.newVariable("uniform", UNIFORM, INT)
    geom.newVariable("varying", VARYING, INT)
    geom.newVariable("vertex", VERTEX, INT)
    geom.newVariable("facevarying", FACEVARYING, INT)
    geom.newVariable("facevertex", FACEVERTEX, INT)
    geom.newVariable("user", USER, INT, user_n=12)

# checkVars
def checkVars(self, geom, uniform, var, vtx, facevar, facevtx):
    """Check the sizes of the variables.
    """
    
    user_slot = geom.slot("user")
    self.assertEqual(user_slot.size(), 12)
    
    c_slot = geom.slot("constant")
    self.assertEqual(c_slot.size(), 1)

    u_slot = geom.slot("uniform")
    self.assertEqual(u_slot.size(), uniform)

    v_slot = geom.slot("varying")
    self.assertEqual(v_slot.size(), var)

    vx_slot = geom.slot("vertex")
    self.assertEqual(vx_slot.size(), vtx)

    fv_slot = geom.slot("facevarying")
    self.assertEqual(fv_slot.size(), facevar)

    fvx_slot = geom.slot("facevertex")
    self.assertEqual(fvx_slot.size(), facevtx)

# checkVarResize
def checkVarResize(self, geom):
    user_slot = geom.slot("user")
    user_slot.resize(15)
    self.assertEqual(user_slot.size(), 15)
    user_slot.resize(12)
    self.assertEqual(user_slot.size(), 12)
    
    c_slot = geom.slot("constant")
    self.assertRaises(ValueError, lambda: c_slot.resize(17))

    u_slot = geom.slot("uniform")
    self.assertRaises(ValueError, lambda: u_slot.resize(u_slot.size()+12))

    v_slot = geom.slot("varying")
    self.assertRaises(ValueError, lambda: v_slot.resize(v_slot.size()+12))

    vx_slot = geom.slot("vertex")
    self.assertRaises(ValueError, lambda: vx_slot.resize(vx_slot.size()+12))

    fv_slot = geom.slot("facevarying")
    self.assertRaises(ValueError, lambda: fv_slot.resize(fv_slot.size()+12))

    fvx_slot = geom.slot("facevertex")
    self.assertRaises(ValueError, lambda: fvx_slot.resize(fvx_slot.size()+12))

    
