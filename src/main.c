// Joseph Hanger

// TODO: construct a miter structure from the two input bench files
// TODO: get the Tseitin transformation of the miter structure
// TODO: format the transformation into that of a .dimacs file

#include <stdio.h>

int main(int argc, char ** args)
{
    if (argc != 4)
    {
        printf("ERROR: invalid number of inputs\n");
        return 1;
    }

    FILE * a_bench_fptr;
    FILE * b_bench_fptr;
    FILE * dimacs_fptr;

    a_bench_fptr = fopen(args[1]);

    if (a_bench_fptr == NULL)
    {
        printf("ERROR: could not open A bench file\n");
        return 1;
    }

    b_bench_fptr = fopen(args[2]);

    if (b_bench_fptr == NULL)
    {
        printf("ERROR: could not open B bench file\n");
        return 1;
    }

    dimacs_fptr = fopen(args[3]);

    if (dimacs_fptr == NULL)
    {
        printf("ERROR: could not open dimacs file\n");
        return 1;
    }

    fclose(a_bench_fptr);
    fclose(b_bench_fptr);
    fclose(dimacs_fptr);

    return 0;
}
