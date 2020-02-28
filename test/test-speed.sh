#!/bin/bash

if [ -z $1 ]; then
  echo "Usage: test/test-speed.sh <iterations> [<miner args>]\n"
fi

MYDIR="$(dirname "$(realpath "$0")")"
cmake $MYDIR/../
make -C $MYDIR/../

BLOCK='ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00000000'

FILE_NAME="test-data-$(date +"%m-%d-%Y-%H%M%S")"
FILE_PATH="$MYDIR/$FILE_NAME"

echo "Saving test data in file $FILE_PATH"
echo ""

for i in $(seq $1); do
  cat $MYDIR/header.bin | \
    $MYDIR/../bigolchungus \
      ${@:2} \
      $BLOCK 1>> $FILE_PATH 2>> $FILE_PATH-err

  EXIT_CODE=$?
  if [ $EXIT_CODE -ne 0 ]; then
    echo ""
    echo "Test failed. Check $FILE_PATH-err"
    exit $EXIT_CODE
  fi

  echo "" >> $FILE_PATH
  echo -n "Iteration $i Results so far: "
  python $MYDIR/check-speed.py $FILE_PATH
done

echo ""
echo ""
echo -n "Test complete.  Final results: "
python $MYDIR/check-speed.py $FILE_PATH
echo""
