import os
import subprocess

file_list = ["MooseGuiDocs","Kkit12Documentation","RdesigneurDocumentation"]

DOCUMENTATION_DESTINATION_DIR = '../GUI/'
SOURCE_EXTENSION = '.markdown'
OUTPUT_EXTENSION = '.rst'
for filename in file_list:
    source_file = filename + SOURCE_EXTENSION
    output_file = DOCUMENTATION_DESTINATION_DIR + filename + OUTPUT_EXTENSION
    command = 'pandoc -s {0} -o {1}'.format(source_file, output_file)
    print(command)
    subprocess.call(command.split(' '))
