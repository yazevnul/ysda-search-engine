#!/usr/bin/env python
# coding=utf-8
#
# This may be implemented as a simple Bash one-liner, but we are doing "production quality code".
#
# BTW, I was trying to make this script multithreaded (I mean multiprocessed, of course), but then I
# discovered all delights of multiprocessing programming.

import argparse
import codecs
import collections

import common


def count_from_stream(input_file):
    counter = collections.Counter()
    for input_line in input_file:
        counter.update(input_line.strip().split())

    return counter


def count_from_file(input_file_name):
    with codecs.open(input_file_name, 'r', 'utf-8') as input_file:
        return count_from_stream(input_file)


def print_counter(counter, output_file):
    for key, count in counter.iteritems():
        output_file.write(u'{}\t{}\n'.format(key, count).encode('utf-8'))


def process_input(input_files):
    counter = collections.Counter()
    for input_file in input_files:
        new_counter = count_from_file(input_file)
        counter += new_counter

    return counter


def _parse_options():
    parser = argparse.ArgumentParser(
        'Count words occurences',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    input_group = parser.add_mutually_exclusive_group(required=True)
    input_group.add_argument(
        '-i', '--input',
        dest='input_file',
        metavar='FILE',
        help='Path to file'
    )
    input_group.add_argument(
        '-d', '--dir',
        dest='input_dir',
        metavar='DIR',
        help='Path to directory with files'
    )
    parser.add_argument(
        '-o', '--output',
        default='-',
        dest='output_file',
        metavar='FILE',
        type=argparse.FileType('w'),
        help='Where to save word occurences. format: word \\t frequency'
    )
    return parser.parse_args()


def _main():
    args = _parse_options()
    input_files = None
    if args.input_file is not None:
        input_files = [args.input_file]
    else:
        input_files = [x for x in common.ifiles_in_directory(args.input_dir)]

    counter = process_input(input_files)
    print_counter(counter, args.output_file)


if '__main__' == __name__:
    _main()
