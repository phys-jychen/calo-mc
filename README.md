# CALO-MC, a Simulation Framework for CEPC Calorimeters

## Author
Ji-Yuan CHEN (SJTU; <jy_chen@sjtu.edu.cn>)

## Description
This program was designed for generating Monte Carlo samples of CEPC calorimeters.  With the help of the MC samples, further studies, including PID, can be carried out.

## Environment Set-up
This project requires CMake version >= 3.11.  If you are working on the cluster of INPAC, IHEP, etc., the environment can be easily set up by executing
```shell
source /cvmfs/sft.cern.ch/lcg/views/LCG_97rc4python3/x86_64-centos7-gcc9-opt/setup.sh
```
This command has been included in `setup.sh`.

Then, the environment with CMake 3.14.3, Geant4-10.06-patch-01, and ROOT 6.20/00 is configured.  As long as no compilation errors are raised and the CMake version requirement is met, other versions on the LCG are also acceptable (be careful of the version of Geant4!). :p

## Installation & Compilation
Having set up the environment, this project can be cloned from GitHub and compiled in a normal way:
```shell
git clone git@github.com:phys-jychen/calo-mc.git
cd calo-mc
mkdir build
cd build
cmake ..
make -j100    # Just do it!
source setup.sh
```

## Usage
Execute
```shell
calo -p
```
to create default configuration yaml file, `default.yaml`, in your current directory.

Then, you can modify the configuration file and execute
```shell
calo -c default.yaml
```
to generate MC samples.

While necessary, you can also print help message by executing
```shell
calo -h
```

Every time you log in to the cluster, before the first time of running this program, remember to execute
```shell
source [build]/setup.sh
```

By now, the compilation has been finished.  Generate your YAML file, and have fun! :p

## Acknowledgement & Reference
The author would like to thank Mr Zhen WANG (TDLI/SJTU), for kindly offering his project [cepc-calo](https://github.com/wangz1996/cepc-calo) for the convenience of generating MC samples of CEPC calorimeters.  In this project (CALO-MC), the Geant4 framework was adopted from cepc-calo.
