***********************************************
:mod:`sequence` --- Handling numbered sequences
***********************************************

.. module:: cgkit.sequence
   :synopsis: Handling numbered sequences

The :mod:`sequence` module contains functionality for handling numbered
(file) sequences. The module defines the following classes:

- :class:`SeqString<cgkit.sequence.SeqString>`: This is the fundamental string class
  that everything else relies upon. It treats numbers inside strings as integer
  numbers and it can be used to sort strings numerically and read/write its
  numbers.

- :class:`Sequence<cgkit.sequence.Sequence>`: This class holds a sorted collection
  of names/objects that all belong to the same numbered sequence.

- :class:`Range<cgkit.sequence.Range>`: A class that handles a sequence of (frame) numbers. It is
  similar to the built-in :func:`range()` function but can contain several
  disjoint sub-ranges and it is initialized from a string (which may come from
  the command line, for example).

- :class:`OutputNameGenerator<cgkit.sequence.OutputNameGenerator>`:
  Generates pairs of input/output names where
  the numbers in the output names can be based on the numbers in the input
  names.

- :class:`SeqTemplate<cgkit.sequence.SeqTemplate>`: A template string class that
  can substitute number patterns with actual numbers.

- :class:`CopySequence<cgkit.sequence.CopySequence>`,
  :class:`MoveSequence<cgkit.sequence.MoveSequence>`,
  :class:`SymLinkSequence<cgkit.sequence.SymLinkSequence>`:
  There are some high-level
  classes that can copy, move/rename sequences or create symbolic links to
  the files from a sequence.

The following functions are available in the module:

..  autofunction:: cgkit.sequence.buildSequences

..  autofunction:: cgkit.sequence.glob

..  autofunction:: cgkit.sequence.compactRange

SeqString Objects
=================

..  autoclass:: cgkit.sequence.SeqString
    :members:

Sequence Objects
================

..  autoclass:: cgkit.sequence.Sequence
    :members:

Range Objects
=============

..  autoclass:: cgkit.sequence.Range
    :members:

OutputNameGenerator Objects
===========================

..  autoclass:: cgkit.sequence.OutputNameGenerator
    :members:
    
    *srcSequences* is a list of :class:`Sequence` objects that contain the source
    sequence files that the output sequence is based on. The structure of
    the names (i.e. how many separate numbers are within a name) determines
    how many number patterns the output name may have.
    *dstName* is a string containing the name pattern for building the
    output file names. The syntax of the pattern is determined by the
    :class:`SeqTemplate` class (i.e. you can use ``@`` or ``#`` characters to
    define where the numbers are located and what their padding is. You can also
    use an index to refer to a particular number from the input sequence
    and you can use expressions within curly braces).
    In the simplest case, the name can just be a base name without any
    special characters at all. In this case, a 4-padded number is
    automatically appended which will receive the values from the
    main number sequence in the input files (or the values specified by
    the destination range).
    
    *srcRanges* is a list of :class:`Range` objects that defines which files from
    the source sequence should be considered, everything outside the range
    is ignored. The numbers produced by the range object refers to the
    main sequence number of the input sequence (i.e. the number that varies
    fastest). If no source range is given for a particular sequence, then
    all input files are considered.
    
    *dstRange* may be a :class:`Range` object that provides the main sequence
    number for the output names. In this case, the main number from the input
    sequence is ignored (unless referenced via an expression). If no range
    object is given, the numbers are taken from the input sequence.
    
    *keepExt* is a boolean that indicates whether the file name extension
    should be added automatically if it isn't already part of the output
    name pattern. Note that the extension is *always* added unless the
    output name already contains exactly the expected extension. If the
    output name contains a different extension, the old extension is still
    added. So if you want to be able to let the user rename the extension,
    you must set this flag to False.
    
    *enforceDstRange* is a boolean that indicates whether the number of
    generated name pairs should always match the number of files indicated
    by the (finite) destination range, even when the source files have
    already been exhausted. The default behavior is to abort the sequence
    if there are no more source files. If the destination range is infinite,
    then this flag has no effect and the sequence always ends when there
    are no more source files.
    
    *repeatSrc* is a flag that is only used when *enforceDstRange* is ``True``
    and there are fewer input files than there are values in the destination
    range. If *repeatSrc* is ``True``, the input sequence is repeated from the
    beginning again, otherwise the last name is duplicated.
    
SeqTemplate Objects
===================

..  autoclass:: cgkit.sequence.SeqTemplate
    :members:
    
    *template* is a string that contains substitution patterns. The patterns
    may be composed of a number of ``@`` characters or a ``#`` character.
    Directly following the pattern there may be an optional integer index
    in brackets that refers to a particular source number that will be used
    during the substitution (e.g. ``@@[1]``, ``#[2]``).
    The pattern may also include an entire expression in Python syntax.
    In this case, the above simple expression must be enclosed in curly
    braces (e.g. ``{#[-1]+10}``, ``{2*@@@@}``).

File Sequence Operations
========================

..  autoclass:: cgkit.sequence.CopySequence
    :members:

    See the :class:`SymLinkSequence` class for a description of constructor
    arguments and available methods.
    
..  autoclass:: cgkit.sequence.MoveSequence
    :members:
    
    See the :class:`SymLinkSequence` class for a description of constructor
    arguments and available methods.

..  autoclass:: cgkit.sequence.SymLinkSequence
    :members:
    :inherited-members:
    
    See the :class:`OutputNameGenerator` class for a description of the
    constructor arguments.
