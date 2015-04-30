#!/usr/bin/env python2
# coding=utf-8

__author__ = 'yazevnul'


import argparse
import os


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
        type=lambda x: os.path.abspath(x),
        help='Directory with files'
    )
    parser.add_argument(
        '-o', '--output',
        dest='output_file',
        metavar='FILE',
        default='-',
        type=argparse.FileType('w'),
        help='Where to save mapping'
    )
    return parser.parse_args()


def ifiles_in_directory(directory_name):
    for file_name in os.listdir(directory_name):
        abs_file_name = os.path.abspath(os.path.join(directory_name, file_name))
        if os.path.isfile(abs_file_name):
            yield abs_file_name


def process_input(directory_name, output_file):
    for abs_file_name in ifiles_in_directory(directory_name):
        file_name = os.path.basename(abs_file_name)
        file_size = os.path.getsize(abs_file_name)
        output_file.write('{}\t{}\n'.format(file_name, file_size))


def _main():
    args = _parse_options()
    process_input(args.input_directory, args.output_file)


if '__main__' == __name__:
    _main()
