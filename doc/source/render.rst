.. % Render tool


The render tool
===============

The tool :file:`render.py` reads one or more files and renders the scene using a
RenderMan renderer. The tool takes care of creating the RIB files and the
shaders, compiling the shaders and launching the  renderer.

If the environment variable :envvar:`RENDER_DEFAULT_OPTIONS` exists, it is read
and parsed to set the default options. After that the options in the command
line are parsed.

**Usage:**

::

   render.py [options] inputfiles

**Options:**

``-f<int>`` / ``--fps=<int>``
   Specifies the number of frames per second (default: 30).

``-h`` / ``--help``
   Print a help message.

``-W<int>`` / ``--width=<int>``
   Specify the width in pixels of the window/screen (default: 640).

``-H<int>`` / ``--height=<int>``
   Specify the height in pixels of the window/screen (default: 480). If no height
   is specified it is computed based on the width and an aspect ratio of 4/3.

``-v`` / ``--verbose``
   Output info messages.

``-p<str>`` / ``--plugin=<str>``
   If the argument specifies a file this file is loaded as a plugin, if it is a
   directory, all files in this directory are loaded. You can specify this option
   more than once or you can use a comma separated list of names.

   You can also set files and paths via the environment variable
   :envvar:`CGKIT_PLUGIN_PATH`. The files/paths have to be separated either by ':'
   or ';'.

``-c<str>`` / ``--camera=<str>``
   Select the camera that should be used to view the scene. The argument is the
   name of the camera. If no camera is specified the first camera found in the
   scene is used.

``-b<time>`` / ``--begin=<time>``
   Specify a time or frame where the animation/simulation should be started
   (default: 0s). You can add the time unit such as 's' for seconds or 'f' for
   frames. Frames are assumed if you don't specify a unit.

``-e<time>`` / ``--end=<time>``
   Specify a time or frame where the animation/simulation should be stopped. You
   can add the time unit such as 's' for seconds or 'f' for frames.  Frames are
   assumed if you don't specify a unit.

``-U<axis>`` / ``--up=<axis>``
   Specify the default value for the up axis. The argument may be either 'y' or
   'z'.

``-O<str>`` / ``--option=<str>``
   Add a global option that will be stored in the scene. The argument should have
   the form "name=value". Using this option is equivalent to writing "Globals(
   name=value )" in a scene file.

``-t`` / ``--set-time``
   Set the starting time directly instead of stepping there from time 0.

``-r<str>`` / ``--renderer=<str>``
   Select the renderer to use (you can choose among ``aqsis``, ``3delight``,
   ``pixie``, ``prman``, ``air`` and ``rdc``).

``-I<str>`` / ``--include=<str>``
   Add the specified include path to the list of paths used for compiling the
   shaders.

``-B`` / ``--bake``
   Activate texture baking mode.

**Events:**

The render tool does not generate any user input events.

