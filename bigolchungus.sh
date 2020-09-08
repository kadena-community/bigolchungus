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
check_set "NODES" "eg: NODES='--node tsundere.waifuwars.org:35090 --node bigchungusmining.energy:4202'"

if [ -z "$CHAINWEB_MINING_CLIENT_EXEC" ]; then
  CHAINWEB_MINING_CLIENT_EXEC=$(which chainweb-mining-client)
  if [ -z "$CHAINWEB_MINING_CLIENT_EXEC" ]; then
    echo "chainweb-mining-client not in PATH and CHAINWEB_MINING_CLIENT_EXEC not set."
    echo "Place the chainweb-mining-client executable in your path."
    exit 1 
  fi
fi

LS_RES=$(ls $CHAINWEB_MINING_CLIENT_EXEC)

if [ $? -ne 0 ]; then
  echo "Invalid CHAINWEB_MINING_CLIENT_EXEC location. ($CHAINWEB_MINING_CLIENT_EXEC)"
  exit 2
fi

CL_INFO_LINES=$(clinfo -l | wc -l | grep -oE '^[0-9]+')

if [ "$CL_INFO_LINES" -eq 0 ]; then
  echo 'No opencl devices found. Run "opencl -l". If nothing shows up, or you get command not found, install opencl'
  echo 'drivers for your graphics card, as well as clinfo.  See the troubleshooting section of the readme for more info.'
  exit 3
fi

MINER_ARGS="-k $MYDIR/kernels/kernel.cl $@"

$CHAINWEB_MINING_CLIENT_EXEC \
  $NODES \
  --log-level debug \
  --public-key $PUBLIC_KEY \
  --worker external \
  --tls \
  --insecure \
  --external-worker-cmd "$MYDIR/bigolchungus $MINER_ARGS"
