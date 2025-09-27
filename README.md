# Circuit Equivalence Checker
Verifies whether two combinational circuits are equivalent
## Build and Run instructions
To build to program go into the project directory and enter
make build
The program takes two input *.bench files which specify the circuits primary inputs, primary outputs, and gates
The output of the program is a *.dimacs file which is the standard input format for most SAT solvers
To run the program enter
./bin/ec example_A.bench example_B.bench example.dimacs
After making the *.dimacs file use it the input to the MiniSat program included in the /util directory
To check whether the *.dimacs file is satifactory or not enter
./util/MiniSat_v1.14_linux example.dimacs example.output
