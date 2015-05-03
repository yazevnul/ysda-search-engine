#!/usr/bin/env python
# coding=utf-8

__author__ = 'yazevnul'


import argparse
import os

import common


def _parse_options():
    parser = argparse.ArgumentParser(
        description='Create table with mapping from file to its size',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        '-d', '--directory',
        dest='input_directory',
        metavar='DIR',
        required=True,
        type=common.existing_directory,
        help='Directory with files'
    )
    parser.add_argument(
        '-o', '--output',
        default='-',
        dest='output_file',
        metavar='FILE',
        type=argparse.FileType('w'),
        help='Where to save mapping'
    )
    return parser.parse_args()


def process_input(directory_name, output_file):
    for abs_file_name in common.ifiles_in_directory(directory_name):
        file_name = os.path.basename(abs_file_name)
        file_size = os.path.getsize(abs_file_name)
        output_file.write('{}\t{}\n'.format(file_name, file_size))


def _main():
    args = _parse_options()
    process_input(args.input_directory, args.output_file)


if '__main__' == __name__:
    _main()
