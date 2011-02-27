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
    """Main function.
    """
    parser = optparse.OptionParser(usage="%prog [options] src dst")
    parser.add_option("-s", "--source-frames", default="0-", metavar="FRAMES", help="Specify a subset of the source frames")
    parser.add_option("-d", "--destination-frames", default=None, metavar="FRAMES", help="Specify the destination numbers")
    parser.add_option("-e", "--drop-extensions", action="store_true", default=False, help="Don't handle missing file extensions in the output pattern")
    parser.add_option("-S", "--symlink", action="store_true", default=False, help="Create symbolic links instead of copying the files")
    parser.add_option("-R", "--realpath", action="store_true", default=False, help="Use the real path of the source files (follow links). This is only relevant when symbolic links are created.")
    parser.add_option("-f", "--force", action="store_true", default=False, help="Never query the user for confirmation")
    parser.add_option("-t", "--test", action="store_true", default=False, help="Only print what would be done, but don't copy anything")
    parser.add_option("-v", "--verbose", action="store_true", default=False, help="Print every file when it is copied")
    parser.add_option("-V", "--version", action="store_true", default=False, help="Display version information")
    opts,args = parser.parse_args()

    if opts.version:
        print ("seqcp (cgkit %s)"%cgkit.cgkitinfo.version)
        sys.exit(0)

    if len(args)!=2:
        parser.print_usage()
        return

    # The source frame numbers
    srcRange = sequence.Range(opts.source_frames)
    
    # The destination frame numbers
    dstRange = None
    if opts.destination_frames is not None:
        dstRange = sequence.Range(opts.destination_frames)

    srcSeq = args[0]
    dstArg = args[1]
    
    # Determine the source sequences
    fseqs = sequence.glob(srcSeq)
    
    if opts.symlink:
        processor = sequence.SymLinkSequence(fseqs, dstArg, [srcRange], dstRange,
                                             keepExt=not opts.drop_extensions, verbose=opts.verbose, resolveSrcLinks=opts.realpath)
        opStr = "Link"
    else:
        processor = sequence.CopySequence(fseqs, dstArg, [srcRange], dstRange,
                                          keepExt=not opts.drop_extensions, verbose=opts.verbose, resolveSrcLinks=opts.realpath)
        opStr = "Copy"
    
    for srcSeq,dstSeq in processor.sequences():
        print ("%s: %s -> %s"%(opStr, srcSeq, dstSeq))
    
    # Check a file number would get appended to a trailing number in the base name
    if processor.mergesNumbers() and not opts.force:
        print ("WARNING: The destination name ends in a number which would affect the output sequence number.")
        if not opts.test and not promptUser("Are you sure to continue?"):
            return

    # Check if an existing file would get overwritten
    overwrites = list(processor.overwrites())
    if len(overwrites)>0 and not opts.force:
        print ("WARNING: %s files would get overwritten."%len(overwrites))
        if not opts.test and not promptUser("Are you sure to continue?"):
            return

    # Dry run or real run...
    if opts.test:
        processor.dryRun()
    else:
        processor.run()

    # TODO: If forward/backward copy fails, the files need to be copied to a temporary sequence first and then renamed.

##########################################################################
try:
    main()
except SystemExit:
    pass
except KeyboardInterrupt:
    sys.stderr.write("\nUser abort\n")
except:
    sys.stderr.write("ERROR: %s\n"%sys.exc_info()[1])
#    raise
    sys.exit(1)
