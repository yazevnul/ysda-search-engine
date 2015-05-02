#!/usr/bin/env python2

__author__ = 'yazevnul'

# pip install beautifulsoup4
# pip install lxml
# pip install nltk

import argparse
import bs4
import nltk
import urllib


def extract_text(html):
    bs = bs4.BeautifulSoup(html, 'lxml')
    unwanted_tags = ['script', 'style']
    for node in bs.find_all(unwanted_tags):
        node.extract()

    text = u' '.join(bs.stripped_strings)
    return text.lower()


def clean_text(text):
    return u' '.join(nltk.tokenize.regexp_tokenize(text, r'\w+'))


def extract_clean_text(html):
    text = extract_text(html)
    return clean_text(text)


def _parse_options():
    parser = argparse.ArgumentParser(
        'Extract text from Wikipedia HTML article',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    input_ = parser.add_mutually_exclusive_group()
    input_.add_argument(
        '-i', '--input',
        dest='input_file',
        metavar='FILE',
        type=argparse.FileType('r'),
        help='File with Wikipedia HTML article'
    )
    input_.add_argument(
        '--url',
        dest='input_url',
        metavar='URL',
        help='Wikipedia article URL'
    )
    parser.add_argument(
        '-o', '--output',
        default='-',
        dest='output_file',
        metavar='FILE',
        type=argparse.FileType('w'),
        help='Where to save extracted text'
    )
    parser.add_argument(
        '--clean',
        dest='clean_text',
        action='store_true',
        help='Clean text after extraction'
    )
    return parser.parse_args()


def _main():
    args = _parse_options()
    html = None
    if args.input_file is not None:
        html = args.input_file.read()
        args.input_file.close()
    else:
        html = urllib.urlopen(args.input_url).read()

    text = None
    if args.clean_text:
        text = extract_clean_text(html)
    else:
        text = extract_text(html)

    args.output_file.write(text)


if '__main__' == __name__:
    _main()
