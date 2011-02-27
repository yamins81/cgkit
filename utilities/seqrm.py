#!/usr/bin/env python
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is the Python Computer Graphics Kit.
#
# The Initial Developer of the Original Code is Matthias Baas.
# Portions created by the Initial Developer are Copyright (C) 2004
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

import sys
import optparse
import os
import os.path
import cgkit.cgkitinfo
from cgkit import sequence

def promptUser(question):
    """Print a question and ask for y/n.
    
    Returns True when the user entered 'y'.
    """
    while 1:
        print question,
        answer = raw_input()
        answer = answer.lower()
        if answer=="n":
            return False
        if answer=="y":
            return True
        print ("Expected 'y' or 'n'")

def main():
    parser = optparse.OptionParser(usage="%prog [options] sequences")
    parser.add_option("-f", "--force", action="store_true", default=False, help="Never query the user for confirmation")
    parser.add_option("-v", "--verbose", action="store_true", default=False, help="Print every file when it is deleted")
    parser.add_option("-V", "--version", action="store_true", default=False, help="Display version information")
    opts,args = parser.parse_args()

    if opts.version:
        print ("seqrm (cgkit %s)"%cgkit.cgkitinfo.version)
        sys.exit(0)

    if len(args)<1:
        parser.print_usage()
        return
 
    # Collect all specified sequences...
    sequences = []
    for pattern in args:
        fseqs = sequence.glob(pattern)
        if len(fseqs)==0:
            print >>sys.stderr, '%s: No sequence "%s" found.'%(os.path.basename(sys.argv[0]), pattern)
            
        for fseq in fseqs:
            sequences.append(fseq)
            
    # No sequences specified? Then there's nothing to do
    if len(sequences)==0:
        return 0
            
    # Ask for confirmation...
    if not opts.force:
        for fseq in sequences:
            print fseq
        if len(sequences)==1:
            msg = "Are you sure to delete the above sequence?"
        else:
            msg = "Are you sure to delete the above %s sequences?"%len(sequences)
        if not promptUser(msg):
            return 0
            
    # Delete the sequences..
    for fseq in sequences:
        print ("Deleting %s"%fseq)
        for fileName in fseq:
            if opts.verbose:
                print ("Deleting %s"%fileName)
            os.remove(str(fileName))
            
    return 0
    
##########################################################################

##########################################################################

try:
    ret = main()
except SystemExit:
    ret = 1
except KeyboardInterrupt:
    sys.stderr.write("\nUser abort\n")
    ret = 1
except:
    sys.stderr.write("ERROR: %s"%sys.exc_info()[1])
#    raise
    ret = 1

sys.exit(ret)

