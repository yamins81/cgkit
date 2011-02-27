.. % Viewer tool


The interactive viewer tool
===========================

The tool :file:`viewer.py` reads one or more files and displays the scene using
a simple OpenGL renderer. It creates events such as keyboard, mouse or joystick
events that can be used by the components in the scene.

If the environment variable :envvar:`VIEWER_DEFAULT_OPTIONS` exists, it is read
and parsed to set the default options. After that the options in the command
line are parsed.

**Usage:**

::

   viewer.py [options] inputfiles

To exit the viewer press :kbd:`Escape` or close the window. How to navigate in
the scene depends on the navigation mode (see option -N).

**Options:**

``-f<int>`` / ``--fps=<int>``
   Specifies the frame rate that should be tried to maintain while playing back the
   scene (default: 30).

``-h`` / ``--help``
   Print a help message.

``-F`` / ``--full-screen``
   Open a full screen display.

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

``-S<str>`` / ``--stereo=<str>``
   Activate stereo display. The argument specifies the method that should be used
   to create a stereo image. It can be either *vsplit* or *glstereo*.

``-D<float>`` / ``--eye-distance=<float>``
   Default eye distance for stereo display (default 0.07). This distance value is
   used if the camera does not explicitly specify a distance value (i.e. if it has
   no attribute *eye_distance*).

``-B`` / ``--bounding-box``
   Display the bounding boxes.

``-P<str>`` / ``--polygon-mode=<str>``
   Specify how polygons should be rendered. Possible values are *fill* (default),
   *line* and *point*.

``-s<str>`` / ``--save=<str>``
   When this option is specified each frame is saved under the given name (+ frame
   number). The extension determines the image format.

``-N<str>`` / ``--navigation-mode=<str>``
   Specify which navigation mode to activate. The viewer can emulate the navigation
   of a few common 3D packages. Possible values are *Maya* (default), *MAX* and
   *Softimage* (case-insensitive).

   In Maya mode you navigate by pressing the :kbd:`Alt` key in combination with any
   of the three mouse buttons. In MAX mode you press the middle mouse button either
   alone or in combination with :kbd:`Alt` or :kbd:`Control` and :kbd:`Alt`. In
   Softimage mode, only the Softimage Navigation Tool is emulated (it's as if this
   tool is permanently active), i.e. you navigate by using one of the three mouse
   buttons.

``-X`` / ``--disable-spacedevice``
   Disables support for SpaceMouse/SpaceBall. This option can be used if there are
   any problems with the driver or initialization takes too long.

``-T`` / ``--disable-wintab``
   Disables tablet support. This option can be used if there are any problems with
   the driver or initialization takes too long.

**Events:**

The viewer tool generates the following user input events (see the module
:mod:`cgkit.events` for more details about these events):

* ``KEY_PRESS``

* ``KEY_RELEASE``

* ``LEFT_DOWN``

* ``MIDDLE_DOWN``

* ``RIGHT_DOWN``

* ``MOUSE_BUTTON_DOWN``

* ``LEFT_UP``

* ``MIDDLE_UP``

* ``RIGHT_UP``

* ``MOUSE_BUTTON_UP``

* ``MOUSE_WHEEL``

* ``MOUSE_MOVE``

* ``JOYSTICK_AXIS``

* ``JOYSTICK_BALL``

* ``JOYSTICK_HAT``

* ``JOYSTICK_BUTTON_DOWN``

* ``JOYSTICK_BUTTON_UP``

* ``SPACE_MOTION``

* ``SPACE_BUTTON_DOWN``

* ``SPACE_BUTTON_UP``

* ``SPACE_BUTTON_ZERO``

* ``TABLET``

**Timing:**

The operations per frame are as follows (in this order):

#. Render and display the current scene at time *t*

#. Handle events

#. Step frame (i.e. increase the time by *dt* and signal the
   ``STEP_FRAME`` event)

#. Sync to the specified framerate

