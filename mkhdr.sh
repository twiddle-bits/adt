#!/usr/bin/env sh

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-of-hdr>"
    exit 1;
fi

set -e

cd `dirname $0`
curdir=`pwd`
cd -

hdrname=`basename $1`
hdrdirpath=`dirname $1`
hdrdirname=`basename $hdrdirpath`

incl_guard=`echo $hdrname | tr '[:upper:]' '[:lower:]' | sed 's/\./_/g'`;
incl_guard=`printf "%s_%s" $hdrdirname $incl_guard`

echo "Creating header $hdrname with include guard $incl_guard"

printf "/*! $hdrname */
/* Copyright (c) `date +%Y` Aaditya Kalsi */

#ifndef $incl_guard
#define $incl_guard

#endif/*$incl_guard*/
" > $1
