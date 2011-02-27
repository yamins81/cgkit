
:mod:`keydefs` --- Key code definitions
=======================================

.. module:: cgkit.keydefs
   :synopsis: Defines toolkit independent key codes


This module defines symbolic names for the key codes that are stored in
:class:`KeyEvent` objects.

The ``mods`` attribute in the :class:`KeyEvent` class is composed of the
following four bits:


.. data:: KEYMOD_SHIFT

   Shift bit in the modifier flags.


.. data:: KEYMOD_CONTROL

   Control bit in the modifier flags.


.. data:: KEYMOD_ALT

   Alt bit in the modifier flags.


.. data:: KEYMOD_META

   Meta bit in the modifier flags.

The ``keycode`` attribute in the :class:`KeyEvent` class can contain one of the
following values::

   KEY_BACK = 8
   KEY_TAB = 9
   KEY_RETURN = 13
   KEY_ESCAPE = 27
   KEY_SPACE = 32

   KEY_CAPSLOCK = 2700

   KEY_SHIFT_LEFT = 2701
   KEY_SHIFT_RIGHT = 2702
   KEY_CONTROL_LEFT = 2703
   KEY_CONTROL_RIGHT = 2704
   KEY_ALT_LEFT = 2705
   KEY_ALT_RIGHT = 2706

   KEY_PRINT = 2707
   KEY_SCROLL = 2708
   KEY_PAUSE = 2709

   KEY_INSERT = 2710
   KEY_DELETE = 127
   KEY_HOME = 2712
   KEY_END = 2713
   KEY_PRIOR = 2714
   KEY_NEXT = 2715

   KEY_LEFT = 2716
   KEY_UP = 2717
   KEY_RIGHT = 2718
   KEY_DOWN = 2719

   KEY_F1 = 2720
   KEY_F2 = 2721
   KEY_F3 = 2722
   KEY_F4 = 2723
   KEY_F5 = 2724
   KEY_F6 = 2725
   KEY_F7 = 2726
   KEY_F8 = 2727
   KEY_F9 = 2728
   KEY_F10 = 2729
   KEY_F11 = 2730
   KEY_F12 = 2731
   KEY_F13 = 2732
   KEY_F14 = 2733
   KEY_F15 = 2734
   KEY_F16 = 2735
   KEY_F17 = 2736
   KEY_F18 = 2737
   KEY_F19 = 2738
   KEY_F20 = 2739
   KEY_F21 = 2740
   KEY_F22 = 2741
   KEY_F23 = 2742
   KEY_F24 = 2743

   KEY_NUMLOCK = 2744

   KEY_NUMPAD0 = 2745
   KEY_NUMPAD1 = 2746
   KEY_NUMPAD2 = 2747
   KEY_NUMPAD3 = 2748
   KEY_NUMPAD4 = 2749
   KEY_NUMPAD5 = 2750
   KEY_NUMPAD6 = 2751
   KEY_NUMPAD7 = 2752
   KEY_NUMPAD8 = 2753
   KEY_NUMPAD9 = 2754

   KEY_NUMPAD_DECIMAL = 2755
   KEY_NUMPAD_ENTER = 2756
   KEY_NUMPAD_ADD = 2757
   KEY_NUMPAD_SUBTRACT = 2758
   KEY_NUMPAD_MULTIPLY = 2759
   KEY_NUMPAD_DIVIDE = 2760

   KEY_WINDOWS_LEFT = 2761
   KEY_WINDOWS_RIGHT = 2762
   KEY_WINDOWS_MENU = 2763

