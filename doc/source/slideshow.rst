.. % SlideShow component


:class:`SlideShow` --- Displaying a series of images
====================================================

The :class:`SlideShow` class can be used to display image files as a slide show.
The component sets up a scene and displays an image sequence with user defined
transitions.


.. class:: SlideShow(name = "SlideShow",  slides = [],  auto_insert = True)

   *slides* is either a string specifying the image files (the string may contain
   wildcards) or a list of :class:`Slide` objects where each object represents one
   or more images.

Example::

   # File: slides.py

   SlideShow(
       slides = [
                 Slide("image01.jpg", XFade(1.0) ),
                 Slide("image02.jpg", XFade(1.0, 0.3) ),
                 Slide("image03.jpg", XCube(2.0) ),
                 Slide("presentation/slides*.png", XFade(0.5) )
                ]
   )

The slide show is started with the viewer tool like this::

   viewer.py slides.py -f50 -F

In this case, the frame rate is increased to 50 frames per second (to get
smoother transitions) and the display is set to full screen.

When the slide show is running you can jump to the next slide by pressing a
mouse button, the :kbd:`Enter` key or the :kbd:`PageDown` key. In case you move
the camera, you can reset it with the :kbd:`q` key.

.. % ----------------------------------------------------------------------


Slide class
-----------

The :class:`Slide` class represents one or more image files and contains one
transition that is used for all files.


.. class:: Slide(filepattern, transition = XCube())

   *filepattern* specifies the image files to load and may include wildcards to
   select more than one file.

   *transition* is a transition class that determines the transition that is
   applied after each image in this slide object.

.. % ----------------------------------------------------------------------


XFade transition
----------------

The :class:`XFade` transition implements a smooth cross fade between two images.


.. class:: XFade(duration=2.0, zmove=0.0)

   *duration* is the length of the transition in seconds.

   If *zmove* is greater than 0, the old image will be moved towards the camera
   during the transition which makes it scale up when viewed with a perspective
   camera.

.. % ----------------------------------------------------------------------


XCube transition
----------------

The :class:`XCube` class implements a transition where the images seem to be
attached on two adjacent sides of a cube and the cube rotates during the
transition.


.. class:: XCube(duration=2.0)

   *duration* is the length of the transition in seconds.

