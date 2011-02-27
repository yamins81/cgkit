######################################################################
# Create a shadow module
######################################################################

import sys, types, optparse

usage = "usage: %prog [options] modulename"
optparser = optparse.OptionParser(usage)
optparser.add_option("-m", "--message", metavar="MSG", default="",
                     help="Add an info message for the exception")
opts, args = optparser.parse_args()

if len(args)<1:
    optparser.print_help()
    sys.exit()

modname = args[0]
message = opts.message

######################################################################

print >>sys.stderr, "Processing module '%s'..."%modname

# Import module...
exec "import %s"%modname
exec "mod = %s"%modname

# Output header...
print """# Shadow module for module "%s"

try:

    # Try to import the original module...
    from %s import *
    
except ImportError:

    # Create dummy symbols...
"""%(modname, modname)

# Create shadow symbols...

callables = []
vars = []

# Fill the callables and vars list
for objname in dir(mod):
    # Ignore everything with an underscore...
    if objname[:1]=="_":
        continue
    obj = getattr(mod, objname)
    # Ignore modules...
    if type(obj)==types.ModuleType:
        continue
    
    if callable(obj):
        callables.append((objname, obj))
    else:
        vars.append((objname, obj))

# Variables...
for objname, obj in vars:
    print '    %s = %s'%(objname, repr(obj))

print ""

# Callables
for objname, obj in callables:
    print '    def %s(*args, **keyargs):'%objname
    msg = "No module named %s."%modname
    if message!="":
        msg += " "+message
    print '        raise ImportError("%s")\n'%msg

