#!/bin/bash
MYDIR="$(dirname "$(realpath "$0")")"

check_set() {
  if [ -z "${!1}" ]; then
    echo "$1 not set."
    echo $2
    exit 4
  fi
}

check_set "PUBLIC_KEY"
check_set "ACCOUNT_NAME"
check_set "NODES" "eg: NODES='--node tsundere.waifuwars.org:35090 --node bigchungusmining.energy:4202'"

if [ -z "$CHAINWEB_MINER_EXEC" ]; then
  CHAINWEB_MINER_EXEC=$(which chainweb-miner)
  if [ -z "$CHAINWEB_MINER_EXEC" ]; then
    echo "chainweb-miner not in PATH and CHAINWEB_MINER_EXEC not set."
    echo "Either place chainweb-miner in your path, or set CHAINWEB_MINER_PATH to the location of the chainweb-miner"
    echo "executable."
    exit 1 
  fi
fi

LS_RES=$(ls $CHAINWEB_MINER_EXEC)

if [ $? -ne 0 ]; then
  echo "Invalid CHAINWEB_MINER_EXEC location. ($CHAINWEB_MINER_EXEC)"
  exit 2
fi

CL_INFO_LINES=$(clinfo -l | wc -l | grep -oE '^[0-9]+')

if [ "$CL_INFO_LINES" -eq 0 ]; then
  echo 'No opencl devices found. Run "opencl -l". If nothing shows up, or you get command not found, install opencl'
  echo 'drivers for your graphics card, as well as clinfo.  See the troubleshooting section of the readme for more info.'
  exit 3
fi

MINER_ARGS="-k $MYDIR/kernels/kernel.cl $@"

$CHAINWEB_MINER_EXEC gpu \
  $NODES \
  --log-level debug \
  --miner-key $PUBLIC_KEY \
  --miner-account $ACCOUNT_NAME \
  --miner-path $MYDIR/bigolchungus \
  --miner-args "$MINER_ARGS"
