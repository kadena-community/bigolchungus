MYDIR="$(dirname "$(realpath "$0")")"

DEVICE_OVERRIDE=0
PLATFORM_OVERRIDE=0
QUIET=1
LOCAL_WORK_SIZE=256 # If you wind up with err -54 in the error file, adjust this. Find optimum value in `clinfo`
WORK_SET_SIZE=64
GLOBAL_SIZE=16777216 # (1024*1024*16)

usage() {
  echo "
  bigolchungus.sh [ -d <device id>        ]
                  [ -p <platform id>      ]
                  [ -l <local work size>  ]
                  [ -w <work set size     ]
                  [ -g <global work size> ]
                  [ -v                    ]
                  <block>

  1. Device Selection

    -d
      set \`device id\`.  Default \`0\`

    -p
      set \`platform id\`.  Default \`0\`

    Run \`clinfo -l\` to get info about your device and platform ids.

  2. Work size configuration 

    -l
      set \`local work size\`. Default \`256\`.
      If you are on AMD, \`256\` is probably the best value for you.
      If you are on nVidia, you probably want \`1024\`.

    -w
      set \`work set size\`. You should never have to modify this. Default \`64\`

    -g
      set \`global work size\`. You should never have to modify this. Default \`16777216\` (1024 * 1024 * 16)

  3. Debugging

    -v
      enable verbose mode.

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
  $DEVICE_OVERRIDE       \
  $PLATFORM_OVERRIDE     \
  $QUIET                 \
  $LOCAL_WORK_SIZE       \
  $WORK_SET_SIZE         \
  $GLOBAL_SIZE           \
  ${@} 2>> err
