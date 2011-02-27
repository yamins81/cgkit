
:mod:`cgkitinfo` --- Release information
========================================

.. module:: cgkit.cgkitinfo
   :synopsis: Provides version information.


New in version 1.1. This module contains two variables that carry information
about the version of the kit.


.. data:: version

   A string containing version information of this release.


.. data:: version_info

   A 4-tuple (*major*, *minor*, *micro*, *releaselevel*) containing the components
   of the version number. The first three values are integers, whereas
   *releaselevel* is a string containing "cvs", "alpha", "beta" or "final".

