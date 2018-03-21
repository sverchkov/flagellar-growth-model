#!/bin/bash

echo original:
echo start:
date
./run -a ../params/default.txt -b ../ic/ic8.dat 15000 -a test/orig_out.txt 30 test/orig_back
echo end:
date

echo threaded:
echo start:
date
./run-threaded -a ../params/default.txt -b ../ic/ic8.dat 15000 -a test/threaded_out.txt 30 test/threaded_back
echo end:
date

