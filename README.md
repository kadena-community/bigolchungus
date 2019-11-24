# Big Ol' Chungus


## Table of Contents

  * [Installation](#installation)
    * [Automated Building](#automated-building) (**Ubuntu only, recommended.**)
    * [Manual Building](#manual-building) 
  * [Running](#running)
    * [Manual](#manual)
    * [Systemd](#systemd) (**recommended**)
  * [Issues](#issues)
  * [Troubleshooting](#troubleshooting)
  * [Contributing](#contributing)

## Installation

#### Automated Building

Currently automatic building is supported only in Ubuntu, and only for running via `systemd`.  If you do not want to
use systemd, or you are not on ubuntu, [build bigolchungus manually](#manual-building).

The setup script has been tested on 18.04 LTS and has worked seamlessly in the past with nvidia hardware.  It is 
untested with AMD, and there have been reports of ubuntu not correctly installing AMD drivers via the command used in 
the script.

There are two requirements for using the automated installation:
  * Ubuntu 18.04
  * You want to use Systemd

If you meet those requirements, the set of commands below should be sufficient to install Big Ol Chungus.

```bash
wget https://raw.githubusercontent.com/kadena-community/bigolchungus/master/setup-miner.sh && \
  chmod +x ./setup-miner.sh && \
  sudo ./setup-miner.sh
```

This will install the required dependencies, set up a user for the `bigolchungus` daemon with the name `kadena-miner`, 
walk you through configuring your miner, then reboot.

After this is done, see [systemd](#systemd).

#### Manual Building 

To build bigolchungus, first you need to make sure you have all of the necessary dependencies.

In ubuntu these dependencies are:
  * ocl-icd-\*
  * opencl-headers
  * clinfo
  * g++
  * librocksdb5.8
  * ubuntu-drivers-common
  * cmake
  * A chainweb-miner binary
  * OpenCL GPU Drivers

If you are on ubuntu, you can install them via

```sh
apt-get update
apt install -y git ocl-icd-* opencl-headers clinfo
apt install -y g++
apt install -y librocksdb5.8 ubuntu-drivers-common
apt install -y cmake
ubuntu-drivers autoinstall --gpgpu
```

Then you'll also need the chainweb miner. You can obtain a release or build from source here: https://github.com/kadena-io/chainweb-miner/

After installing dependencies, the project can be built via

```sh
cmake .
make
```

You should see a file called `bigolchungus` get created.  

You can test your build by running `test/test.sh` from the project root.

You can set up systemd from here if you like.  There is a sample systemd service file in the `resources` directory.

## Running


#### Manual

The simplest and most common invocation of the miner will be

```sh
chainweb-miner gpu \
  --node my.node:port \
  --miner-key $MINER_KEY \
  --miner-account $MINER_ACCOUNT \
  --log-level debug \
  --miner-path /path/to/BigOlChungus/bigolchungus
  --miner-args "-k /path/to/BigOlChungus/kernels/kernel.cl
```

This will run the miner on the first GPU that OpenCL detects.  You can configure the GPU which `bigolchungus` runs on by
appending `-p $platform_number -d $device_number` to the end of `--miner-args`.  Run `clinfo -l` to see your GPUs and 
their respective platforms.

Run `bigolchungus -h` for more information about configuration.


#### Systemd

If you set up `bigolchungus` via the `setup-miner.sh` script in the [automated build](#automated-building), or you 
manually set up systemd using the sample systemd service file, you should now have a systemd service named 
`kadena-miner@` which you can run and enable to automatically run your miner.

```
sudo systemctl enable kadena-miner@0 # Make the miner run on startup for GPU 0
sudo systemctl start kadena-miner@0 # Make the miner start running now on GPU 0
```

For running the miner on a different GPU, just change the number after the `@` in the above commands.

To check the logs, run `journalctl -u kadena-miner@<gpu-id>`

## Issues

  * Each GPU currently takes a full CPU core.  If you wish to run 2 GPUs, you must have at least 2 CPU cores available.
  * Running multiple GPUs requires running multiple instances of the miner
  * OpenCL errors are cryptic and `chainweb-miner` does not have clear output.

## Troubleshooting

Run `test/test.sh` from the project root.

  * If you get an output where the last line is just 3 numbers separated by spaces, everything is fine.
  * If you see something like "No OpenCL Platforms Detected" Then your issue is with drivers. Run `opencl -l` to verify. 
    If you see no output, then drivers are definitely not installed correctly. Try installing drivers and then rebooting.
  * If you see error -30 or -46, then you likely have a permissions issue, or you have deleted the `kernel.cl` file in 
    the `kernels` directory.  Make sure that file exists, and that your current user has permission to access it.

## Contributing

See our [contributing guidelines](./CONTRIBUTING.md)
