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

#define MAX_INPUTS 64
#define MAX_OUTPUTS 64
#define MAX_GATE_INPUTS 64
#define MAX_GATES 2048
#define MAX_NAME 128

enum GATE_TYPE { AND, NAND, OR, NOR, NOT, XOR, BUFF, UNKNOWN };

typedef struct {
    enum GATE_TYPE type;
    int input_cnt;
    char inputs[MAX_GATE_INPUTS][MAX_NAME];
    char output[MAX_NAME];
} gate_t;

typedef struct {
    int input_cnt;
    int output_cnt;
    int gate_cnt;
    char inputs[MAX_INPUTS][MAX_NAME];
    char outputs[MAX_OUTPUTS][MAX_NAME];
    gate_t gates[MAX_GATES];
} circuit_t;

static circuit_t * parse_bench(const char * file_name);
static circuit_t * miter_struct(circuit_t * a, circuit_t * b);
static void print_circuit(const circuit_t  * cir);
//static struct circuit miter(struct circuit cir_a, struct circuit cir_b);
//static int tseitin_transform(struct circuit cir);

int main(int argc, char ** args)
{
    if (argc != 4)
    {
        printf("ERROR: invalid number of inputs\n");
        return 1;
    }

    circuit_t * a = parse_bench(args[1]);
    circuit_t * b = parse_bench(args[2]);
    circuit_t * miter = miter_struct(a, b);
    print_circuit(a);
    print_circuit(b);
    print_circuit(miter);

    return 0;
}

void print_circuit(const circuit_t * cir)
{
    printf("\n*** circuit ***\n");
    printf("input count: %d\n", cir->input_cnt);
    printf("output count: %d\n", cir->output_cnt);
    printf("gate count: %d\n", cir->gate_cnt);
    printf("\n");

    // print inputs
    for (int i = 0; i < cir->input_cnt; i++)
    {
        printf("INPUT(%s)\n", cir->inputs[i]);
    }

    printf("\n");

    // print outputs
    for (int i = 0; i < cir->output_cnt; i++)
    {
        printf("OUTPUT(%s)\n", cir->outputs[i]);
    }

    printf("\n");

    // print gates
    for (int i = 0; i < cir->gate_cnt; i++)
    {
        printf("%s = ", cir->gates[i].output);
        printf("%d(", cir->gates[i].type);

        for (int j = 0; j < cir->gates[i].input_cnt - 1; j++)
        {
            printf("%s, ", cir->gates[i].inputs[j]);
        }

        printf("%s)\n", cir->gates[i].inputs[cir->gates[i].input_cnt - 1]);
    }
}

circuit_t * parse_bench(const char * file_name)
{
    circuit_t * cir;
    FILE * fptr;
    char line[256];
    char * token;
    size_t len;

    fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        printf("ERROR: could not open file %s\n", file_name);
        return NULL;
    }

    // initialize circuit;
    cir = calloc(1, sizeof(circuit_t));
    if (cir == NULL)
    {
        printf("ERROR: circuit memory allocation failed\n");
        return NULL;
    }

    while(fgets(line, sizeof(line), fptr) != NULL)
    {
        len = strlen(line);

        // remove newline
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            len--;
        }

        // skip empty lines and comments
        if (len == 0 || line[0] == '#')
        {
            continue;
        }

        if (strncmp(line, "INPUT", 5) == 0)
        {
            token = strtok(line, "(");
            token = strtok(NULL, ")");

            strcpy(cir->inputs[cir->input_cnt], token);
            cir->input_cnt++;
        }
        else if (strncmp(line, "OUTPUT", 6) == 0)
        {
            token = strtok(line, "(");
            token = strtok(NULL, ")");

            strcpy(cir->outputs[cir->output_cnt], token);
            cir->output_cnt++;
        }
        else
        {
            // format: output_pin = GATE(input1, input2, ...)

            gate_t * gate = &cir->gates[cir->gate_cnt];
            gate->input_cnt = 0;

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

            while (token != NULL)
            {
                strcpy(gate->inputs[gate->input_cnt], token);
                gate->input_cnt++;
                token = strtok(NULL, ", )");
            }

            // add gate to circuit
            cir->gate_cnt++;
        }
    }

    fclose(fptr);

    return cir;
}

// shares primary inputs from circuits a and b
circuit_t * miter_struct(circuit_t * a, circuit_t * b)
{
    if (a->input_cnt != b->input_cnt)
    {
        printf("ERROR: inputs do not match\n");
    }

    if (a->output_cnt != b->output_cnt)
    {
        printf("ERROR: outputs do not match\n");
    }

    // initialze miter
    circuit_t * m;
    m = calloc(1, sizeof(circuit_t));

    // copy primary inputs
    m->input_cnt = a->input_cnt;
    memcpy(m->inputs, a->inputs, sizeof(a->inputs));

    // copy gates from circuit_a with prefix "a_"
    for (int i = 0; i < a->gate_cnt; i++)
    {
        gate_t * g = &m->gates[m->gate_cnt];
        *g = a->gates[i]; // copy struct

        // rename gate output
        snprintf(g->output, MAX_NAME, "a_%s", a->gates[i].output);

        // rename gate inputs if they are not primary inputs
        for (int j = 0; j < g->input_cnt; j++)
        {
            int is_primary = 0;

            for (int k = 0; k < a->input_cnt; k++)
            {
                if (strcmp(a->gates[i].inputs[j], a->inputs[k]) == 0)
                {
                    is_primary = 1;
                    break;
                }
            }

            if (is_primary == 0)
            {
                snprintf(g->inputs[j], MAX_NAME, "a_%s", a->gates[i].inputs[j]);
            }
            else
            {
                strcpy(g->inputs[j], a->gates[i].inputs[j]);
            }
        }

        m->gate_cnt++;
    }

    // copy gates from b with prefix "b_"
    for (int i = 0; i < b->gate_cnt; i++)
    {
        gate_t * g = &m->gates[m->gate_cnt];
        *g = b->gates[i];

        // rename gate output
        snprintf(g->output, MAX_NAME, "b_%s", b->gates[i].output);

        // rename gate inputs if they are not primary inputs
        for (int j = 0; j < g->input_cnt; j++)
        {
            int is_primary = 0;

            for (int k = 0; k < b->input_cnt; k++)
            {
                if (strcmp(b->gates[i].inputs[j], b->inputs[k]) == 0)
                {
                    is_primary = 1;
                    break;
                }
            }

            if (is_primary == 0)
            {
                snprintf(g->inputs[j], MAX_NAME, "b_%s", b->gates[i].inputs[j]);
            }
            else
            {
                strcpy(g->inputs[j], b->gates[i].inputs[j]);
            }
        }

        m->gate_cnt++;
    }

    // connect all the output pairs to a 2-input XOR gate

    for (int i = 0; i < a->output_cnt; i++)
    {
        gate_t * xor_gate = &m->gates[m->gate_cnt];
        memset(xor_gate, 0, sizeof(gate_t));
        xor_gate->type = XOR;
        xor_gate->input_cnt = 2;

        snprintf(xor_gate->inputs[0], MAX_NAME, "a_%s", a->outputs[i]);
        snprintf(xor_gate->inputs[1], MAX_NAME, "b_%s", b->outputs[i]);
        snprintf(xor_gate->output, MAX_NAME, "xor_out_%d", i);

        m->gate_cnt++;
    }

    // connect all of the XOR gates or a single OR

    gate_t * or_gate = &m->gates[m->gate_cnt];
    or_gate->type = OR;
    or_gate->input_cnt = a->output_cnt;

    for (int i = 0; i < a->output_cnt; i++)
    {
        snprintf(or_gate->inputs[i], MAX_NAME, "xor_out_%d", i);
    }

    snprintf(or_gate->output, MAX_NAME, "miter_out");
    m->gate_cnt++;
    strcpy(m->outputs[0], "miter_out");
    m->output_cnt = 1;

    return m;
}
