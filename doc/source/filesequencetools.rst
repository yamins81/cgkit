*******************
File Sequence Tools
*******************

There is a collection of tools that provide the equivalent functionality
to the Unix command line tools :command:`ls`, :command:`mv`, :command:`cp`,
:command:`rm`, :command:`ln` but that operate on
numbered file sequences instead of individual files. The tools are front-ends
for the functionality provided by the :mod:`sequence<cgkit.sequence>` module.

seqls.py
========

::

    seqls.py [options] pattern

List directory contents where file sequences are listed as one single entry
instead of listing all files individually.

See :ref:`selecting_source_sequence` for more information on how the source
files can be specified. **Note:** Because the input pattern is assumed to
be a name *prefix*, you must add a trailing path separator if you want to
list the contents of a directory. Example::

  # The following line would only display all sequences in the current
  # directory whose name begin with "clips". If there is a sub-directory
  # called "clips", this would not be examined!
  
  > seqls.py clips

  # To see the contents of a directory, you need to add a path separator
  # to the name to indicate that the input pattern refers to a directory,
  # so the following line lists all sequences inside the "clips" directory.
  
  > seqls.py clips/

**Options:**

``-h`` / ``--help``
   Show the help message.
   
``-l`` / ``--long``
   Display additional information per sequence.
    
``-d`` / ``--directories``
   Include directories in the list.

``-V`` / ``--version``
   Display version information and quit.

seqmv.py
========

**Usage:**

::

    seqmv.py [options] src dst

Moves a numbered file sequence to a new location and/or renames the sequence.
All sequences specified as input will be moved to the output sequence.

See :ref:`selecting_source_sequence` for more information on how the source
files can be specified.

See :ref:`specifying_the_destination_sequence_pattern` for more information
about the destination name pattern.

The source and destination sequence may refer to the same file sequence and
the input and output numbers may be overlapping. The tool will determine the
required order of operation so that no input file gets overwritten (in some
cases it may have to move to a temporary sequence first).

By default, the tool will check if an existing file (that is not part of the
input sequence) would get overwritten and ask the user for confirmation if
this is the case. You can use the ``-f/--force`` option to suppress the
confirmation query.

**Options:**

``-h`` / ``--help``
   Show the help message.

``-s FRAMES`` / ``--source-frames=FRAMES``
   Select a subset of frames in the source sequence. The specified ranges
   refer to the main sequence number.
   See :ref:`specifying_frame_ranges` for more information on the frame range
   syntax.

``-d FRAMES`` / ``--destination-frames=FRAMES``
   Specify a new number range for the destination files.
   See :ref:`specifying_frame_ranges` for more information on the frame range
   syntax.

``-e`` / ``--drop-extensions``
   Don't handle missing file extensions in the output name. The file extensions
   of the output sequence must then be part of the destination name pattern.
   You have to use this option if you want the files in the output sequence
   to have a different extension than the files in the source sequence.
   
``-f`` / ``--force``
   Never query the user for confirmation.
    
``-t`` / ``--test``
   Only print what would be done, but don't move anything.

``-v`` / ``--verbose``
   Print every file when it is moved.

``-V`` / ``--version``
   Display version information and quit.

seqcp.py
========

**Usage:**

::

    seqcp.py [options] src dst

Copies a numbered file sequence to a new location or creates links.
All sequences specified as input will be copied to the output sequence.

See :ref:`selecting_source_sequence` for more information on how the source
files can be specified.

See :ref:`specifying_the_destination_sequence_pattern` for more information
about the destination name pattern.

The source and destination sequence may refer to the same file sequence and
the input and output numbers may be overlapping. The tool will determine the
required order of operation so that no (unprocessed) input file gets overwritten.

By default, the tool will check if an existing file (that is not part of the
input sequence) would get overwritten and ask the user for confirmation if
this is the case. You can use the ``-f/--force`` option to suppress the
confirmation query.

**Options:**

``-h`` / ``--help``
   Show the help message.

``-s FRAMES`` / ``--source-frames=FRAMES``
   Select a subset of frames in the source sequence. The specified ranges
   refer to the main sequence number.
   See :ref:`specifying_frame_ranges` for more information on the frame range
   syntax.

``-d FRAMES`` / ``--destination-frames=FRAMES``
   Specify a new number range for the destination files.
   See :ref:`specifying_frame_ranges` for more information on the frame range
   syntax.

``-e`` / ``--drop-extensions``
   Don't handle missing file extensions in the output name. The file extensions
   of the output sequence must then be part of the destination name pattern.
   You have to use this option if you want the files in the output sequence
   to have a different extension than the files in the source sequence.

``-S`` / ``--symlink``
   Create symbolic links instead of copying the files.

``-R`` / ``--realpath``
   When the source files are symbolic links, this option instructs the utility
   to replace the source file names by their real path (i.e. follow links).
   This is only really relevant when the option ``-S/--symlink`` is used.
   
``-f`` / ``--force``
   Never query the user for confirmation.
    
``-t`` / ``--test``
   Only print what would be done, but don't do anything.

``-v`` / ``--verbose``
   Print every file when it is moved.

``-V`` / ``--version``
   Display version information and quit.

seqrm.py
========

**Usage:**

::

    seqrm.py [options] sequences

Deletes the specified sequences.

See :ref:`selecting_source_sequence` for more information on how the source
files can be specified.

By default, the tool will prompt the user for confirmation before deleting
the files. You can use the ``-f/--force`` option to suppress the
confirmation query.

**Options:**

``-h`` / ``--help``
   Show the help message.
   
``-f`` / ``--force``
   Never query the user for confirmation.
    
``-v`` / ``--verbose``
   Print every file when it is moved.

``-V`` / ``--version``
   Display version information and quit.


.. _selecting_source_sequence:

Selecting the Source Sequence
=============================

The source sequence is determined by picking all files that begin with the
pattern specified as *src*, so it is not necessary to provide the full
file name but you have to make sure the prefix is unique. For example,
if the file sequence is named :file:`animation0001.tif`, :file:`animation0002.tif`, ...
you could just pass ``"a"`` as input sequence, assuming there are no other files in the
directory that begin with an ``"a"``. The pattern can also include wild cards
(``"animation*.tif"``) but then you may have to enclose the pattern with quotes
to prevent the shell from expanding the names.

.. _specifying_the_destination_sequence_pattern:

Specifying the Destination Sequence Pattern
===========================================

The destination sequence can be named differently than the input sequence
and it can have a new numbers. The tools support file names that have
multiple numbers in them but only the main sequence numbers (i.e. the number
that varies fastest) can be modified, the other numbers are simply taken
from the input files.

If the output name is just a single name, then a 4-padded number will be
appended to the name automatically. File extensions are preserved as well,
so in the simplest case it is enough to just provide a new base name. 
For example, to rename a sequence frames*.tif to animation*.tif the following
command would be enough (assuming there are no other files that begin with "frames"
and that are not part of the sequence)::

  seqmv.py frames animation     ; rename the frames sequence into animation

To specify the exact location and padding of a number, the destination pattern
may include "#" or "@" characters which are placeholders for the actual numbers.
The number of adjacent "@" characters defines the padding of the number. The
"#" character represents a 4-padded number and is equivalent to "@@@@". Examples:

* ``anim#.tif``: This results in anim0001.tif, anim0002.tif, ...
* ``anim@@@@.tif``: The is the same as above.
* ``anim@@.tif``: anim01.tif, anim02.tif, ...

The number of the placeholders must either match the total number of number
occurences in the input file name or the number of varying number occurences.
A varying number is one that changes throughout the sequence. Otherwise it is
a constant number which may also be treated as part of the name.
For example, if you have files clip1_0001.tif, clip1_0002.tif, clip2_0001.tif,
clip2_0002.tif and you use the source name "clip" (which references all files),
then the output pattern must include two number placeholders whereas if you
use the source name "clip1" (which only references the first clip) then you
may either have two or just one placeholder. In the latter case, the placeholder
will refer to the second number in the input name because that's the varying
number.

By default, the placeholders reference the numbers in the same order as they
are given in the input file names. If you want to rearrange the order of the
numbers (or drop a number) you can append an 1-based index in square brackets 
to a placeholder. Example::

  seqmv.py clip img_#[2]_clip_@@[1]   ; Swap the clip number and frame number

Finally, you can apply an expression to a frame number. In that case, you
have to put the entire placeholder into curly braces::

  seqmv.py clip1 clip{@@+10}_#            ; Add 10 to the clip number
  seqmv.py clip img_#[2]_clip_{@@[1]+10}  ; Swap numbers and increase clip number


.. _specifying_frame_ranges:

Specifying Frame Ranges
=======================

There are two options that take frame ranges as argument, the ``-s/--source-frames``
option which selects a subset of source frames and the ``-d/--destination-frames``
option which assigns new numbers to the destination names.

The frame ranges are specified as a comma-separated list of individual ranges
where a single range is either a single number or a begin/end pair with an
optional step number. Examples:
 
    ============  ==============================================================
    Range         Description
    ============  ==============================================================
    ``1,2,5``     Selects only frames 1, 2 and 5
    ``1-6``       Selects frames 1, 2, 3, 4, 5 and 6
    ``1-3,7-9``   Selects frames 1, 2, 3, 7, 8 and 9.
    ``2-6x2``     Selects frames 2, 4 and 6
    ``10-``       Selects frames 10, 11, 12, ... (until the end of the sequence)
    ============  ==============================================================
    

