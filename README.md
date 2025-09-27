# Circuit Equivalence Checker
Verifies whether two combinational circuits are equivalent. Constructs a miter structure from two circuits and generates its Conjunctive Normal Form (CNF). The CNF is then inputted into a SAT solver which checks if it is satisfiable.

## Build and Run instructions
To build to program go into the project directory and enter:

```make build```

The program takes two input **\*.bench** files which specify the circuits primary inputs, primary outputs, and gates. The output of the program is a **\*.dimacs** file which is the standard input format for most SAT solvers. A *.dimacs file contains a list of Conjunctive Normal Form (CNF) clauses with each '0\n' implying an AND.

To run the program enter:

```./bin/ec example_A.bench example_B.bench example.dimacs```

After making the *.dimacs file use it as input to the MiniSat program included in the **/util** directory to check whether the circuits are equivalent or not.

```./util/MiniSat_v1.14_linux example.dimacs example.output```

