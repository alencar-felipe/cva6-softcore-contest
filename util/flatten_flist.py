#!/bin/env python3
# Copyright 2023 Commissariat a l'Energie Atomique et aux Energies
#                Alternatives (CEA)
#
# Licensed under the Solderpad Hardware License, Version 2.1 (the “License”);
# you may not use this file except in compliance with the License.
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
# You may obtain a copy of the License at https://solderpad.org/licenses/
#
# Authors: Cesar Fuguet
# Date: October, 2023
# Description: script to flatten a Flist file. Flattening consist on:
#   -  expanding environment variables in the file
#   -  expanding included Flist files

import sys
import argparse
import os


def flatten_flist(in_flist, out_flist, print_incdir, print_newline):
    """Recursively parses Flist files, expanding environment variables and
    including files as necessary."""

    for line in in_flist:
        line = line.strip()

        if line.startswith(('#', '//', '/*')):
            continue

        line = os.path.expandvars(line)

        if line.startswith('+incdir+') and print_incdir:
            print(line, file=out_flist, end="\n" if print_newline else " ")

        elif line.startswith('-F'):
            inc_filename = line.lstrip('-F').strip()

            if not os.path.exists(inc_filename):
                raise FileNotFoundError(f'File not found: {inc_filename}')

            with open(inc_filename, 'r', errors='ignore') as inc_flist:
                flatten_flist(
                    inc_flist,
                    out_flist,
                    print_incdir,
                    print_newline
                )

        elif line:
            print(line, file=out_flist, end="\n" if print_newline else " ")


def get_arguments():
    """Parses command line arguments."""

    return parser.parse_args()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=(
        'Flatten a Flist file by expanding includes and environment variables.'
    ))
    parser.add_argument(
        '--print_incdir',
        action="store_true",
        help='Print incdir statements in the output'
    )
    parser.add_argument(
        '--print_newline',
        action="store_true",
        help='Print newline in the output after each line'
    )
    parser.add_argument(
        'in_flist',
        nargs='?',
        default=None,
        help='Input Flist file (default to stdin)'
    )
    parser.add_argument(
        'out_flist',
        nargs='?',
        default=None,
        help='Output flattened Flist file (default to stdout)'
    )

    args = parser.parse_args()

    in_flist = (
        open(args.in_flist, 'r', errors='ignore')
        if args.in_flist else
        sys.stdin
    )

    out_flist = (
        open(args.out_flist, 'w')
        if args.out_flist else
        sys.stdout
    )

    try:
        flatten_flist(
            in_flist,
            out_flist,
            args.print_incdir,
            args.print_newline
        )
    finally:
        if in_flist is not sys.stdin:
            in_flist.close()
        if out_flist is not sys.stdout:
            out_flist.close()
