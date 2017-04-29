#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

set -e

$root/build.sh
cd $root/tests/install
./test.sh
