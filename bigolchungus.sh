MYDIR="$(dirname "$(realpath "$0")")"

DEVICE_OVERRIDE=$1
PLATFORM_OVERRIDE=0
QUIET=0
LOCAL_WORK_SIZE=256 # If you wind up with err -54 in the error file, adjust this. Find optimum value in `clinfo`
WORK_SET_SIZE=64
GLOBAL_SIZE=16777216 # (1024*1024*16)


exec $MYDIR/bigolchungus \
  $DEVICE_OVERRIDE \
  $PLATFORM_OVERRIDE \
  $QUIET \
  $LOCAL_WORK_SIZE \
  $WORK_SET_SIZE \
  $GLOBAL_SIZE \
  ${@:2} 2>> err
