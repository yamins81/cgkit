#!/usr/bin/env python

import SilverCity
from docutils import io, nodes, statemachine, utils
from docutils.parsers.rst import Directive, directives
import StringIO

class CodeBlock(Directive):

   """
   The code-block directive provides syntax highlighting for blocks
   of code.  It is used with the the following syntax::
  
   .. code-block:: CPP
     
      #include <iostream>
    
      int main( int argc, char* argv[] )
      {
        std::cout << "Hello world" << std::endl;
      }
    
   The directive requires the name of a language supported by SilverCity
   as its only argument.  All code in the indented block following
   the directive will be colourized.  Note that this directive is only
   supported for HTML writers.

   The directive can also be told to include a source file directly::

   .. code-block::
      :language: Python
      :source-file: ../myfile.py

   You cannot both specify a source-file and include code directly.
   """

   optional_arguments = 1
   final_argument_whitespace = True
   option_spec = {"language" : directives.unchanged,
                  "source-file" : directives.path,}
   has_content = True

   def run(self):
      
       try:
           language = self.arguments[0]
       except IndexError:
           language = self.options["language"]

       if self.content and "source-file" in self.options:
           error = self.state_machine.reporter.error( 
               "You cannot both specify a source-file and include code directly.",
               nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
           return [error]
           
       if not self.content:
           try:
               self.content = [line.rstrip() for line in file(self.options["source-file"])]
           except KeyError:
               # source-file was not specified
               pass
           except IOError:
               error = self.state_machine.reporter.error( 
                   "Could not read file %s." %self.options["source-file"],
                   nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
               return [error]
 
       try:
           module = getattr(SilverCity, language)
           generator = getattr(module, language+"HTMLGenerator")
       except AttributeError:
           error = self.state_machine.reporter.error( "No SilverCity lexer found "
                                                      "for language ''%s''." %language, 
							nodes.literal_block(self.block_text, self.block_text), line=self.lineno )
           return [error]
       io = StringIO.StringIO()
       generator().generate_html( io, "\n".join(self.content) )
       html = '<div class="code-block">\n%s\n</div>\n' % io.getvalue()
       raw = nodes.raw('',html, format = 'html')
       return [raw]


directives.register_directive("code-block", CodeBlock)
