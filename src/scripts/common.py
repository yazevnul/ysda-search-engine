# coding=utf-8

import os


def existing_directory(path):
    if not os.path.isdir(path):
        raise RuntimeError('Directory {} doesn\'t exists'.format(path))
    return os.path.abspath(path)


def positive_integer(value_str):
    value = int(value_str)
    if value <= 0:
        raise RuntimeError('{} must be positive integer'.format(value))
    return value
