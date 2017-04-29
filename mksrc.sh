#!/usr/bin/env sh

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-of-hdr>"
    exit 1;
fi

set -e

cd `dirname $0`
curdir=`pwd`
cd -

srcname=`basename $1`
srcdirpath=`dirname $1`
srcdirname=`basename $srcdirpath`

echo "Creating source $srcname"

printf "/*! $srcname */
/* Copyright (c) `date +%Y` Aaditya Kalsi */

" > $1
