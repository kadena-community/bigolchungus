#!/bin/sh

apt-get update
apt install -y ocl-icd-* opencl-headers clinfo
apt install -y g++
apt install -y librocksdb5.8 ubuntu-drivers-common
apt install -y cmake

mkdir /home/kadena-miner
cd /home/kadena-miner
wget https://github.com/kadena-io/chainweb-node/releases/download/1.0.2/chainweb.8.6.5.ubuntu-18.04.fdf9d0e3.tar
tar -xzf chainweb.8.6.5.ubuntu-18.04.fdf9d0e3.tar
useradd kadena-miner
# OpenCL
ubuntu-drivers autoinstall --gpgpu

git clone https://github.com/kadena-community/bigolchungus.git /home/kadena-miner/BigOlChungus
cd /home/kadena-miner/BigOlChungus
cmake .
make
cp -r /home/kadena-miner/BigOlChungus/kernels /home/kadena-miner

cat <<EOF > /etc/systemd/system/kadena-miner@.service
[Unit]
Description=Big Ol Chungus On %I

[Service]
EnvironmentFile=/home/kadena-miner/env
User=kadena-miner
WorkingDirectory=/home/kadena-miner
ExecStart=/home/kadena-miner/chainweb-miner gpu \$NODES --miner-key \$PUBLIC_KEY --miner-account \$PUBLIC_KEY --log-level debug \$CHAIN --miner-path /home/kadena-miner/BigOlChungus/bigolchungus.sh --miner-args %I
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
EOF

# NODES="--node x.y.z:p --node ..."
# CHAIN="--chain n" or empty
cat <<EOF > /home/kadena-miner/env
NODES=
PUBLIC_KEY=
CHAIN=
EOF

chown -R kadena-miner /home/kadena-miner/
reboot

# after running:
# download the miner to /home/kadena-miner/bigolchungus
# make sure /home/kadena-miner/kernels contains kernel.cl
# `systemctl start kadena-miner@0` starts the miner on GPU 0, etc
# `systemctl enable kadena-miner@0` makes the miner for GPU 0 start with the system, etc
# `journalctl -u kadena-miner@0` shows logs for the miner on GPU 0, etc, add `--follow` to follow them as they come in
