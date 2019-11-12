MYDIR="$(dirname "$(realpath "$0")")"

DEVICE_OVERRIDE=0
PLATFORM_OVERRIDE=0
QUIET=1
LOCAL_WORK_SIZE=256 # If you wind up with err -54 in the error file, adjust this. Find optimum value in `clinfo`
WORK_SET_SIZE=64
GLOBAL_SIZE=16777216 # (1024*1024*16)

usage() {
  echo "
  bigolchungus.sh [ -d <device id>        ] \ # default 0
                  [ -p <platform id>      ] \ # default 0
                  [ -l <local work size>  ] \ # default 256
                  [ -w <work set size     ] \ # default 64
                  [ -g <global work size> ] \ # default 16777216 (1024 * 1024 * 16)
                  [ -v                    ]   # verbose mode
                  <block>

  Run \`clinfo\` to get your device and platform ids.

  You should never have to modify \`work set size\` or \`global work size\`,  so -g and -w can be left alone.
  
  If you are on nVidia, you probably want \`-l 1024\`. 

  If you are on AMD, the default is good.
  "
}

while getopts "d:p:l:w:g:vh" options; do
  case "${options}" in
    d)
      DEVICE_OVERRIDE=${OPTARG}
      ;;
    p)
      PLATFORM_OVERRIDE=${OPTARG}
      ;;
    l)
      LOCAL_WORK_SIZE=${OPTARG}
      ;;
    w)
      WORK_SET_SIZE=${OPTARG}
      ;;
    g)
      GLOBAL_SIZE=${OPTARG}
      ;;
    v)
      QUIET=0
      ;;
    h)
      usage
      exit 0
      ;;
    ?)
      usage
      exit 1
      ;;
  esac
done
shift $(expr $OPTIND - 1)

exec $MYDIR/bigolchungus \
  $DEVICE_OVERRIDE \
  $PLATFORM_OVERRIDE \
  $QUIET \
  $LOCAL_WORK_SIZE \
  $WORK_SET_SIZE \
  $GLOBAL_SIZE \
  ${@} 2>> err
