#!/bin/bash
MYDIR="$(dirname "$(realpath "$0")")"
cmake $MYDIR/../
make -C $MYDIR/../
cat $MYDIR/header.bin | \
  $MYDIR/../bigolchungus \
    ${@} ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00000000
exit $?
