#!/bin/bash
MYDIR="$(dirname "$(realpath "$0")")"
cmake $MYDIR/../
make -C $MYDIR/../
cat $MYDIR/header.bin | \
  $MYDIR/../bigolchungus \
    -k $MYDIR/../kernels/kernel.cl \
    ${@} ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00000000
exit $?
