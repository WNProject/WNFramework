#!/usr/bin/python
import argparse
import os
import subprocess
import sys

def main():
  parser = argparse.ArgumentParser(
        description="Update scripting grammar")
  parser.add_argument("--java", type=str, help="Java executable", default="java")
  parser.add_argument("--antlr", type=str, help="Antlr3 location", default="antlr3.jar")
  parser.add_argument("script_file", type=str)
  args = parser.parse_args(sys.argv[1:])
  source_location = os.path.abspath(os.path.join(args.script_file, '..', '..', 'src'));
  script_name = os.path.splitext(os.path.split(args.script_file)[-1])[0]
  subprocess.check_call(
    [args.java, '-jar', args.antlr, os.path.abspath(args.script_file), '-o',
     source_location])

  files = [ os.path.join(source_location, script_name + x) for x in
    ['.tokens', 'Lexer.hpp', 'Lexer.cpp', 'Parser.hpp', 'Parser.cpp']]

  last_licence_line = "// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
  source_line = "From the grammar source file :"
  source_date = "                          On :"
  file_name = "static const char fileName[] = \""

  for file in files:
    write_lines = []
    with open(file) as my_f:
      has_read_licence = False
      insert_clang_format_off = False
      in_comment = False
      for line in my_f.readlines():
        if last_licence_line in line:
          has_read_licence = True
          insert_clang_format_off = True
        if source_line in line:
          continue
        if source_date in line:
          continue
        if has_read_licence:
          if (file_name in line):
            line = file_name + script_name + ".g\";"
          if line.lstrip().startswith('//'):
            continue
          line_left = line
          line = ""
          while(line_left):
            if not in_comment and '/*' in line_left:
              vals = line_left.split('/*', 1)
              line += vals[0]
              line_left = vals[1]
              in_comment = True
              continue
            if in_comment and '*/' in line_left:
              line_left = line_left.split('*/', 1)[1]
              in_comment = False
              continue
            if not in_comment:
              line += line_left
            line_left = ""
        line = line.replace("ImplTraits::CommonTokenType*", "ImplTraits::CommonTokenType const*")
        write_lines.append(line.rstrip() + '\n')
        if insert_clang_format_off:
          write_lines.append("// clang-format off\n")
          insert_clang_format_off = False
    with open(file, "w") as my_f:
      my_f.writelines(write_lines)

if __name__ == "__main__":
  main()
