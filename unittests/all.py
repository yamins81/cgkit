#!/usr/bin/env python
# Do all available tests

import glob, os.path, unittest

# A list of module names that should be ignored
ignore = []

# Import all test_*.py files
for test in glob.glob("test_*.py"):
    modname, ext = os.path.splitext(test)
    if modname in ignore:
        print "IGNORE: %s"%modname
    else:
        print "Importing %s..."%modname
        exec "from %s import *"%modname

# Do the test
print "Starting test"
unittest.main()
