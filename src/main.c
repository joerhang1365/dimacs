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
static void print_circuit(const struct circuit cir);
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

    print_circuit(a);

    return 0;
}

static void print_circuit(const struct circuit cir)
{
    printf("*** circuit ***\n");
    printf("\n*** inputs ***\n");
    printf("input count: %d\n", cir.input_cnt);

    for (int i = 0; i < cir.input_cnt; i++)
    {
        printf("input %d: %s;\n", i, cir.inputs[i]);
    }

    printf("\n*** outputs ***\n");
    printf("output count: %d\n", cir.output_cnt);

    for (int i = 0; i < cir.output_cnt; i++)
    {
        printf("output %d: %s;\n", i, cir.outputs[i]);
    }

    printf("\n*** gates ***\n");
    printf("gate count: %d\n", cir.gate_cnt);

    for (int i = 0; i < cir.gate_cnt; i++)
    {
        printf("gate type: %d\n", cir.gates[i].type);
        printf("gate input count: %d\n", cir.gates[i].input_cnt);

        for (int j = 0; j < cir.gates[i].input_cnt; j++)
        {
            printf("gate input %d: %s;\n", j, cir.gates[i].inputs[j]);
        }

        printf("gate output: %s;\n\n", cir.gates[i].output);
    }
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

    // initialize circuit
    cir->input_cnt = 0;
    cir->output_cnt = 0;
    cir->gate_cnt = 0;

    while(fgets(line, sizeof(line), fptr) != NULL)
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
            // format: output_pin = GATE(input1, input2, ...)

            struct gate * gate = &cir->gates[cir->gate_cnt];
            char * token;

            // get gate output pin
            token = strtok(line, " =");
            strcpy(gate->output, token);

            // get gate type

            token = strtok(NULL, "= (");

            if (strncmp(token, "AND", 3) == 0)
            {
                gate->type = AND;
            }
            else if (strncmp(token, "NAND", 4) == 0)
            {
                gate->type = NAND;
            }
            else if (strncmp(token, "OR", 2) == 0)
            {
                gate->type = OR;
            }
            else if (strncmp(token, "NOR", 3) == 0)
            {
                gate->type = NOR;
            }
            else if (strncmp(token, "NOT", 3) == 0)
            {
                gate->type = NOT;
            }
            else if (strncmp(token, "XOR", 3) == 0)
            {
                gate->type = XOR;
            }
            else if (strncmp(token, "BUFF", 3) == 0)
            {
                gate->type = BUFF;
            }

            // get gate inputs

            token = strtok(NULL, ", )");

            do
            {
                strcpy(gate->inputs[gate->input_cnt], token);
                gate->input_cnt++;
                token = strtok(NULL, ", )");
            }
            while (token != NULL);

            // add gate to circuit
            cir->gate_cnt++;
        }
    }

    fclose(fptr);
}
