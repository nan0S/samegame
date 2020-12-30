#!/bin/python

import glob
from natsort import natsorted

def get_boards_header():
    return 'int boards[][15 * 15] = {\n'

def get_boards_body():
    boards = ''
    for name in natsorted(glob.glob('tests/*.txt'))[:20]:
        with open(name) as file:
            content = file.read()[:-2]
            c = content.replace(' ', ',').replace('\n', '\n\t  ')
            boards += '\n\t{ ' + c + ' },\n'
    return boards

def get_boards_footer():
    return  '};\n'

def get_boards():
    return get_boards_header() + \
           get_boards_body() + \
           get_boards_footer()

def get_results_header():
    return 'std::vector<std::pair<int, int>> results[] = {\n'

def get_results_body():
    results = ''
    for name in natsorted(glob.glob('results/*.txt'))[:20]:
        results += '\t{\n'
        with open(name) as file:
            for line in file.readlines():
                results += '\t  {' + line.strip().replace(' ', ',') + '},\n'
        results += '\t},\n'
    return results

def get_results_footer():
    return '};\n'

def get_results():
    results = get_results_header()
    results += get_results_body()
    results += get_results_footer()
    return results

boards = get_boards()
results = get_results()

source = open('skeleton.cpp').read()
print(source.replace('<#boards#>', boards).replace('<#results#>', results))