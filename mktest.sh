#!/usr/bin/env sh

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <test-name>"
    exit 1;
fi

set -e

cd `dirname $0`
curdir=`pwd`
cd -

testname="$1.cpp"

suite_name=`echo $testname | tr '[:upper:]' '[:lower:]' | sed 's/\./_/g'`;

echo "Creating test tests/$testname with suite $suite_name"

printf "/*! $testname */
/* Copyright (c) `date +%Y` Aaditya Kalsi */

#include \"defs.h\"

defineSuite($suite_name)
{
    testThat(0 == 0);
}
" > tests/$testname

echo "Appending suite to tests/suites.h"
printf "runSuite($suite_name);\n" >> $curdir/tests/suites.h

echo "New suites.h:"
cat $curdir/tests/suites.h
