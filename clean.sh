#!/usr/bin/env sh
exec=`realpath $0`
root=`dirname $exec`

set -e

rm -fr $root/_build $root/_install
