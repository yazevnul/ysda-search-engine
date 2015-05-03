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


def ifiles_in_directory(directory_name):
    for file_name in os.listdir(directory_name):
        abs_file_name = os.path.abspath(os.path.join(directory_name, file_name))
        if os.path.isfile(abs_file_name):
            yield abs_file_name
