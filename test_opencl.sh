MYDIR="$(dirname "$(realpath "$0")")"
exec $MYDIR/test_opencl $@ 2>> err
