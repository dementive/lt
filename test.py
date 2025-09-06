#!/usr/bin/env python
import os
import re
import subprocess

# Config
enabled_tests = {}
ignored_tests = {'tests'}

# Generate tests
hpp_files = []
test_functions = []
files_with_tests = set()
test_case_declaration_pattern = re.compile(r'^(test|utest)\((?P<function_name>[A-Za-z_0-9][A-Za-z_0-9]*),\n$')

for root, _, files in os.walk('./test'):
    hpp_files.extend([os.path.join(root, f) for f in files if f.endswith('.hpp')])

for filepath in hpp_files:
    with open(filepath, 'r', encoding='utf-8') as file:
        for line_num, line in enumerate(file, 1):
            match = test_case_declaration_pattern.match(line)
            if not match:
                continue

            function_name = match.group('function_name')
            if function_name not in ignored_tests and (len(enabled_tests) == 0 or function_name in enabled_tests):
                test_functions.append(function_name)
                files_with_tests.add(filepath)

hpp_files = [x.split("/")[-1] for x in hpp_files if x in files_with_tests]
includes = [f'#include "{file}"' for file in hpp_files]
test_main_call = f'test_main({", ".join(test_functions)})'
output = '\n'.join(includes + [''] + [test_main_call])

with open('./test/main.cpp', 'w', encoding='utf-8') as f:
    f.write(output)

# Compile tests
os.chdir('build')
subprocess.run(['cmake', '..'])
subprocess.run(['cmake', '--build', '.'])

# Run tests
subprocess.run(['./test'])
