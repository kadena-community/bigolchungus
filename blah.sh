#!/bin/bash

echo "Configuring miner..."
EXAMPLE_NODE="--node fr1.chainweb.com:443"

while [ -z $NODES ]; do
  echo -n "Enter the nodes you wish to connect to [example = $EXAMPLE_NODE]: "
  read NODES
done

if [ "$NODES" != *"--node"* ]; then
  NODES="--node $NODES"
fi

echo $NODES
