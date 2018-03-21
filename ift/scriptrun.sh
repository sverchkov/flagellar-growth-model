#!/bin/bash

#sleep 30
./run -a ../params/nbar10M.txt -b ../ic/ic4.dat 15000 -b data/ens100-19.dat 250 data/ens100.backup
./run -a ../params/nbar40M.txt -b ../ic/ic4.dat 15000 -b data/ens400-19.dat 250 data/ens400.backup
./run -a ../params/nbar90M.txt -b ../ic/ic4.dat 15000 -b data/ens900-19.dat 250 data/ens900.backup
./run -a ../params/nbar10M.txt -b ../ic/ic4.dat 15000 -b data/ens100-20.dat 250 data/ens100.backup
./run -a ../params/nbar40M.txt -b ../ic/ic4.dat 15000 -b data/ens400-20.dat 250 data/ens400.backup
./run -a ../params/nbar90M.txt -b ../ic/ic4.dat 15000 -b data/ens900-20.dat 250 data/ens900.backup
date
