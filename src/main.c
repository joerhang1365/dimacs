// Joseph Hanger

// TODO: construct a miter structure from the two input bench files
//  check if both circuits have same number of inputs and outputs
//  rename every variable with a preceding a_ or b_
//  add an xor connecting both circuits same numbered output
//  add an or connecting all of the xor outputs
//  create a new bench for testing the miter structure creation
// TODO: get the Tseitin transformation of the miter structure
// TODO: format the transformation into that of a .dimacs file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUTS 100
#define MAX_OUTPUTS 100
#define MAX_GATE_INPUTS 100
#define MAX_GATES 1000
#define MAX_NAME 16

enum GATE_TYPE { AND, NAND, OR, NOR, NOT, XOR, BUFF };

struct gate {
    enum GATE_TYPE type;
    int input_cnt;
    char inputs[MAX_GATE_INPUTS][MAX_NAME];
    char output[MAX_NAME];
};

struct circuit {
    int input_cnt;
    int output_cnt;
    int gate_cnt;
    char inputs[MAX_INPUTS][MAX_NAME];
    char outputs[MAX_OUTPUTS][MAX_NAME];
    struct gate gates[MAX_GATES];
};

static void parse_bench(const char * file_name, struct circuit * cir);
//static struct circuit miter(struct circuit cir_a, struct circuit cir_b);
//static int tseitin_transform(struct circuit cir);

int main(int argc, char ** args)
{
    if (argc != 4)
    {
        printf("ERROR: invalid number of inputs\n");
        return 1;
    }

    struct circuit a;
    struct circuit b;

    parse_bench(args[1], &a);
    parse_bench(args[2], &b);

    printf("input count: %d\n", a.input_cnt);

    for (int i = 0; i < a.input_cnt; i++)
    {
        printf("input %d: %s\n", i, a.inputs[i]);
    }

    printf("output count: %d\n", a.output_cnt);

    for (int i = 0; i < a.output_cnt; i++)
    {
        printf("output %d: %s\n", i, a.outputs[i]);
    }

    return 0;
}

static void parse_bench(const char * file_name, struct circuit * cir)
{
    FILE * fptr;
    char line[256];

    fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        printf("ERROR: could not open file %s\n", file_name);
    }

    while(fgets(line, sizeof(line), fptr))
    {
        // remove newline
        line[strcspn(line, "\n")] = 0;

        // skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '#')
        {
            continue;
        }

        if (strncmp(line, "INPUT", 5) == 0)
        {
            char * input;

            input = strtok(line, "(");
            input = strtok(NULL, ")");

            strcpy(cir->inputs[cir->input_cnt], input);
            cir->input_cnt++;
        }
        else if (strncmp(line, "OUTPUT", 6) == 0)
        {
            char * output;

            output = strtok(line, "(");
            output = strtok(NULL, ")");

            strcpy(cir->outputs[cir->output_cnt], output);
            cir->output_cnt++;
        }
        else
        {
            // check if line is a gate
            // output_pin = GATE(input1, input2, ...)
            struct gate gate;
            char * token;

            // get gate output
            token = strtok(line, " =(,)");
            strcpy(gate.output, token);

            // get gate type
            token = strtok(NULL, " =(,)");
            token = strtok(NULL, " =(,)");
            token = strtok(NULL, " =(,)"); // QUICK FIX

            if (strncmp(token, "AND", 3) == 0)
            {
                gate.type = AND;
            }
            else if (strncmp(token, "NAND", 4) == 0)
            {
                gate.type = NAND;
            }
            else if (strncmp(token, "OR", 2) == 0)
            {
                gate.type = OR;
            }
            else if (strncmp(token, "NOR", 3) == 0)
            {
                gate.type = NOR;
            }
            else if (strncmp(token, "NOT", 3) == 0)
            {
                gate.type = NOT;
            }
            else if (strncmp(token, "XOR", 3) == 0)
            {
                gate.type = XOR;
            }
            else if (strncmp(token, "BUFF", 3) == 0)
            {
                gate.type = BUFF;
            }

            // get gate inputs
            while ((token = strtok(NULL, ",")) != NULL)
            {
                strcpy(gate.inputs[gate.input_cnt], token);
                gate.input_cnt++;
            }

            // add gate to circuit
            cir->gates[cir->gate_cnt] = gate;
            cir->gate_cnt++;
        }
    }

    fclose(fptr);
}
