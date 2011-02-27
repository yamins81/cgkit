#!/usr/bin/env python
# Pack the generated tutorial files for publishing
# Calling this script generates the file "tutorials.tar.gz"

import sys, os

cmd = """tar cfvz tutorials.tar.gz
index.html
default.css
thumbs/*.jpg
first_steps/first_steps.html
first_steps/*.jpg
animation/animation.html
custom_vars/custom_vars.html
materials/materials.html
materials/ambientdiffuse.py
materials/uvmap.png
materials/alphatex.png
materials/texdemo1.py
materials/texdemo3.py
materials/texdemo4.py
materials/*.jpg
renderman/renderman.html
renderman/*.jpg
renderman/simplescene.py
renderman/uvmap.png
ogre/ogre.html
baking/baking.html
baking/model.jpg
baking/ao_map1.jpg
baking/uvlayout.png
baking/final1.jpg
baking/final2.jpg
baking/final_rgb.jpg
baking/final_rgb2.jpg
baking/bunny.jpg
code_examples/demo1.html
code_examples/demo2.html
code_examples/demo3.html
code_examples/shownormals.html
code_examples/*.jpg
"""

os.system(cmd.replace("\n"," "))
