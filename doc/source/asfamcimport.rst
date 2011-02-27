.. % ASF/AMC import


Acclaim Skeleton/Motion Capture file (ASF/AMC) import
-----------------------------------------------------

The ASF/AMC plugin reads Acclaim Skeleton Files (ASF) containing the structure
of a character and Acclaim Motion Capture (AMC) files containing the actual
motion.

You can read an ASF file alone in which case only the skeleton will be read.
When reading an AMC file, the plugin also has to read the corresponding ASF file
as this file contains information how to interpret the data in the AMC file. By
default, the plugin looks for an ASF file with the same name than the AMC file
but with suffix ``.asf`` instead of ``.amc``. If this file doesn't exist the
plugin checks if the directory contains only one ASF file and chooses this one.
If this also fails, an exception is generated and you should pass the ASF file
name explicitly as argument to the :func:`load` function.

The plugin supports the following options that can be passed to the :func:`load`
command when reading AMC files:

+---------------+----------+-------------------------------+
| Option        | Default  | Description                   |
+===============+==========+===============================+
| ``asf``       | ``None`` | The name of the corresponding |
|               |          | skeleton file.                |
+---------------+----------+-------------------------------+
| ``framerate`` | ``30``   | The framerate of the motion   |
|               |          | data in the AMC file.         |
+---------------+----------+-------------------------------+

