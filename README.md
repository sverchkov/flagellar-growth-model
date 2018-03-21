Flagellar Growth Model
======================

A stochastic model of flagellar growth, initial work by Yuriy Sverchkov and Muruhan Rathinam, 2007

Compiling
---------

Run `make` in the `ift` folder, it will generate `run`

Running
-------

### Usage

    Usage: ./run -a|b parameters -a|b input time -a|b output [runs backup]

    Where 'parameters' is the name of the file containing the simulation
    parameters, 'input' is the name of the file containing the initial
    conditions, 'time' is the simulation time limit (in seconds), 'output' is
    the name of the file to output the results to, and 'runs' is the number
    of times to run the simulation.

    The simulation will run in 'ensemble' mode if 'runs' is specified, and in
    'trajectory' mode if it is not.
    If 'runs' is specified, then so must 'backup' be specified - a file to store
    temporary results (those will be stored in binary format).

    -a 	specifies that the file that follows is an ascii file.
    -b 	specified that the file that follows is a binary file.

### Parameters file

The parameters file contains three floating-point values corresponding to lambda+, lambda-, and mu respectively (see paper for details).

### Initial conditions file

The initial conditions file contains, in the following order:

 * Initial flagellar length (integer)
 * Number of IFTs (integer)
 * Position of each IFT (integer)

License and Copyright
---------------------

This repository contains an implementation of the Mersenne Twister algorithm, its licence and copyright are as specified within the corresponding directory.

For the remainder of the code,

   Copyright 2018 Yuriy Sverchkov

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
