#!/usr/bin/env python
# coding=utf-8

__author__ = 'yazevnul'

import argparse
import os
import codecs
import multiprocessing
import logging

from itertools import izip, repeat

import common
import wiki_text_extractor


def extract_text(input_file_name, output_file_name, clean_text):
    logging.info('input: {}, output: {}, clean: {}'.format(
        input_file_name, output_file_name, clean_text
    ))
    text = None
    with codecs.open(input_file_name, 'r', 'utf-8') as input_:
        if clean_text:
            text = wiki_text_extractor.extract_clean_text(input_.read())
        else:
            text = wiki_text_extractor.extract_text(input_.read())

    assert isinstance(text, unicode)

    with codecs.open(output_file_name, 'w', 'utf-8') as output_:
        output_.write(text)


def extract_text_wrapper(tuple_):
    extract_text(*tuple_)


def ifiles_in_directory(directory_name):
    for file_name in os.listdir(directory_name):
        abs_file_name = os.path.abspath(os.path.join(directory_name, file_name))
        if os.path.isfile(abs_file_name):
            yield abs_file_name


def process_input(input_dir_name, output_dir_name, clean_text, jobs_count):
    logging.info('processing_input')
    in_files = [x for x in ifiles_in_directory(input_dir_name)]
    out_files = [os.path.join(output_dir_name, os.path.basename(x)) for x in in_files]
    pool = multiprocessing.Pool(jobs_count)
    jobs = pool.imap_unordered(extract_text_wrapper, izip(in_files, out_files, repeat(clean_text)))
    for _ in jobs:
        pass
    pool.close()
    pool.join()


def _parse_options():
    parser = argparse.ArgumentParser(
        'Extract text from all Wikipedia HTML articles in a given directory',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        '-i', '--input',
        dest='input_dir',
        metavar='DIR',
        required=True,
        type=common.existing_directory,
        help='Directory with Wikipedia articles'
    )
    parser.add_argument(
        '-o', '--output',
        dest='output_dir',
        metavar='DIR',
        required=True,
        type=common.existing_directory,
        help='Where to save extracted texts'
    )
    parser.add_argument(
        '--clean',
        dest='clean_text',
        action='store_true',
        help='Clean text after extraction'
    )
    parser.add_argument(
        '-j', '--jobs',
        default='1',
        dest='jobs_count',
        metavar='INT',
        type=common.positive_integer,
        help='Jobs count'
    )
    return parser.parse_args()


def _main():
    args = _parse_options()
    process_input(args.input_dir, args.output_dir, args.clean_text, args.jobs_count)

if '__main__' == __name__:
    _main()
