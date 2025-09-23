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

#define MAX_NAME_LEN 16
#define MAX_INPUTS 512
#define MAX_OUTPUTS 512
#define MAX_GATE_INPUTS 128
#define MAX_GATES 4096

enum gate_type { AND, NAND, OR, NOR, NOT, XOR, BUFF, UNKNOWN };

struct gate {
    char inputs[MAX_GATE_INPUTS][MAX_NAME_LEN];
    char output[MAX_NAME_LEN];
    int input_cnt;
    enum gate_type type;
};

struct logic_gates {
    char inputs[MAX_INPUTS][MAX_NAME_LEN];
    char outputs[MAX_OUTPUTS][MAX_NAME_LEN];
    struct gate gates[MAX_GATES];
    int input_cnt;
    int output_cnt;
    int gate_cnt;
};

typedef struct {
    char name[MAX_NAME_LEN];
    int id;
} map_t;

static char * gate_name(enum gate_type type);
static void print_bench(struct logic_gates * lg);
static struct logic_gates * parse_bench(const char * file_name);
static struct logic_gates * miter_struct(struct logic_gates * a, struct logic_gates * b);

static int new_id(const char * name);
static int get_id(const char * name);
static void print_map();
static int tseytin_transform(struct logic_gates * lg, const char * file_name);

static map_t * map = NULL;
static int id_cnt = 0;

int main(int argc, char ** args)
{
    if (argc != 4)
    {
        printf("ERROR: invalid number of inputs\n");
        return 1;
    }

    struct logic_gates * a = parse_bench(args[1]);
    struct logic_gates * b = parse_bench(args[2]);
    struct logic_gates * miter = miter_struct(a, b);

    print_bench(a);
    print_bench(b);
    print_bench(miter);

    tseytin_transform(miter, args[3]);

    return 0;
}

char * gate_name(enum gate_type type)
{
    static char * const names[] =
    {
        [AND] = "AND",
        [NAND] = "NAND",
        [OR] = "OR",
        [NOR] = "NOR",
        [NOT] = "NOT",
        [XOR] = "XOR",
        [BUFF] = "BUFF",
        [UNKNOWN] = "UNKNOWN"
    };

    if ((int)type >= 0 && (int)type < sizeof(names) / sizeof(names[0]))
    {
        return names[type];
    }
    else
    {
        return "UNDEFINED";
    }
};

void print_bench(struct logic_gates * lg)
{
    printf("\n*** logic circuit ***\n");
    printf("input count: %d\n", lg->input_cnt);
    printf("output count: %d\n", lg->output_cnt);
    printf("gate count: %d\n", lg->gate_cnt);
    printf("\n");

    // print inputs
    for (int i = 0; i < lg->input_cnt; i++)
    {
        printf("INPUT(%s)\n", lg->inputs[i]);
    }

    printf("\n");

    // print outputs
    for (int i = 0; i < lg->output_cnt; i++)
    {
        printf("OUTPUT(%s)\n", lg->outputs[i]);
    }

    printf("\n");

    // print gates
    for (int i = 0; i < lg->gate_cnt; i++)
    {
        printf("%s = ", lg->gates[i].output);
        printf("%s(", gate_name(lg->gates[i].type));

        for (int j = 0; j < lg->gates[i].input_cnt - 1; j++)
        {
            printf("%s, ", lg->gates[i].inputs[j]);
        }

        printf("%s)\n", lg->gates[i].inputs[lg->gates[i].input_cnt - 1]);
    }
}

struct logic_gates * parse_bench(const char * file_name)
{
    FILE * fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        printf("ERROR: could not open file %s\n", file_name);
        return NULL;
    }

    // initialize circuit;
    struct logic_gates * cir = malloc(sizeof(struct logic_gates));
    cir->input_cnt = 0;
    cir->output_cnt = 0;
    cir->gate_cnt = 0;

    if (cir == NULL)
    {
        printf("ERROR: circuit memory allocation failed\n");
        return NULL;
    }

    char line[256];

    while(fgets(line, sizeof(line), fptr) != NULL)
    {
        size_t len = strlen(line);

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
            char * token = strtok(line, "(");

            if (token == NULL)
            {
                continue;
            }

            token = strtok(NULL, ")");

            if (token == NULL)
            {
                continue;
            }

            strcpy(cir->inputs[cir->input_cnt], token);
            cir->input_cnt++;
        }
        else if (strncmp(line, "OUTPUT", 6) == 0)
        {
            char * token = strtok(line, "(");

            if (token == NULL)
            {
                continue;
            }

            token = strtok(NULL, ")");

            if (token == NULL)
            {
                continue;
            }

            strcpy(cir->outputs[cir->output_cnt], token);
            cir->output_cnt++;
        }
        else
        {
            // format: output_pin = GATE(input1, input2, ...)

            struct gate gate;
            gate.input_cnt = 0;

            // get gate output pin
            char * token = strtok(line, " =");

            if (token == NULL)
            {
                printf("ERROR: could not find gate output\n");
                continue;
            }

            strcpy(gate.output, token);

            // get gate type

            token = strtok(NULL, "= (");

            if (token == NULL)
            {
                printf("ERROR: could not find gate type\n");
                continue;
            }

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
            else
            {
                gate.type = UNKNOWN;
            }

            // get gate inputs

            token = strtok(NULL, ", )");

            while (token != NULL)
            {
                strcpy(gate.inputs[gate.input_cnt], token);
                gate.input_cnt++;
                token = strtok(NULL, ", )");
            }

            if (gate.input_cnt == 0)
            {
                printf("ERROR: could not find inputs for gate\n");
                continue;
            }

            // add gate to circuit
            cir->gates[cir->gate_cnt] = gate;
            cir->gate_cnt++;
        }
    }

    fclose(fptr);

    return cir;
}

// shares primary inputs from circuits a and b
struct logic_gates * miter_struct(struct logic_gates * a, struct logic_gates * b)
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
    struct logic_gates * m = malloc(sizeof(struct logic_gates));
    m->input_cnt = 0;
    m->output_cnt = 0;
    m->gate_cnt = 0;

    // copy primary inputs
    m->input_cnt = a->input_cnt;

    for (int i = 0; i < a->input_cnt; i++)
    {
        strcpy(m->inputs[i], a->inputs[i]);
    }

    // copy gates from circuit_a with prefix "a_"
    for (int i = 0; i < a->gate_cnt; i++)
    {
        // copy gate
        struct gate g = a->gates[i];

        // rename gate output
        sprintf(g.output, "a_%s", a->gates[i].output);

        // rename gate inputs if they are not primary inputs
        for (int j = 0; j < g.input_cnt; j++)
        {
            int is_primary = 0;

            // check if gate input is circuit input
            for (int k = 0; k < a->input_cnt; k++)
            {
                if (strcmp(g.inputs[j], a->inputs[k]) == 0)
                {
                    is_primary = 1;
                    break;
                }
            }

            if (is_primary == 0)
            {
                sprintf(g.inputs[j], "a_%s", a->gates[i].inputs[j]);
            }
        }

        m->gates[m->gate_cnt] = g;
        m->gate_cnt++;
    }

    // copy gates from b with prefix "b_"
    for (int i = 0; i < b->gate_cnt; i++)
    {
        // copy gate
        struct gate g = b->gates[i];

        // rename gate output
        sprintf(g.output, "b_%s", b->gates[i].output);

        // rename gate inputs if they are not primary inputs
        for (int j = 0; j < g.input_cnt; j++)
        {
            int is_primary = 0;

            for (int k = 0; k < b->input_cnt; k++)
            {
                if (strcmp(g.inputs[j], b->inputs[k]) == 0)
                {
                    is_primary = 1;
                    break;
                }
            }

            if (is_primary == 0)
            {
                sprintf(g.inputs[j], "b_%s", b->gates[i].inputs[j]);
            }
        }

        m->gates[m->gate_cnt] = g;
        m->gate_cnt++;
    }

    // connect all the output pairs to a 2-input XOR gate
    // FIXME: right now this works but in future outputs might not
    // be in same index of output array so might need to search for the
    // name and use that index
    for (int i = 0; i < a->output_cnt; i++)
    {
        struct gate xor_gate;
        xor_gate.type = XOR;
        xor_gate.input_cnt = 2;

        sprintf(xor_gate.inputs[0], "a_%s", a->outputs[i]);
        sprintf(xor_gate.inputs[1], "b_%s", b->outputs[i]);
        sprintf(xor_gate.output, "xor_%d", i);

        m->gates[m->gate_cnt] = xor_gate;
        m->gate_cnt++;
    }

    // connect all of the XOR gates to a single OR gate

    struct gate or_gate;
    or_gate.type = OR;
    or_gate.input_cnt = a->output_cnt;

    for (int i = 0; i < a->output_cnt; i++)
    {
        // FIXME: this might be a lazy solution
        sprintf(or_gate.inputs[i], "xor_%d", i);
    }

    strcpy(or_gate.output, "miter");
    m->gates[m->gate_cnt] = or_gate;
    m->gate_cnt++;
    strcpy(m->outputs[0], "miter");
    m->output_cnt = 1;

    return m;
}

int new_id(const char * name)
{
    map = realloc(map, (id_cnt + 1) * sizeof(map_t));
    strcpy(map[id_cnt].name, name);
    map[id_cnt].id = id_cnt + 1;
    id_cnt++;
    return id_cnt;
}

int get_id(const char * name)
{
    // check if id exists
    for (int i = 0; i < id_cnt; i++)
    {
        if (strcmp(map[i].name, name) == 0)
        {
            return map[i].id;
        }
    }

    // make a new id
    return new_id(name);
}

void print_map()
{
    printf("*** map ***\n");

    for (int i = 0; i < id_cnt; i++)
    {
        printf("map[%d]: name='%s', id=%d\n", i, map[i].name, map[i].id);
    }
}

// This solution is a little scuffed as I am manually programming all the
// transfermations
// I am pretty sure I can use a tree to build all of the clauses
// but dont know how to do that yet and need to pass this class
// can make a better later
// TODO: store clauses in a struct so that I can properly count how
// many there are. Then I need to add the last output or something as
// a final clause
// AND: C = A & B ; (~A | ~B | C) & (A | ~C) & (B | ~C)
// OR : C = A | B ; (A | B | ~C) & (~A | C) & (~B | C)

int tseytin_transform(struct logic_gates * const cir, const char * file_name)
{
    FILE * dimacs_fptr = fopen(file_name, "w");

    if (dimacs_fptr == NULL)
    {
        printf("ERROR: could not open .dimacs file\n");
        return 1;
    }

    // assign a unique ID to every input
    for (int i = 0; i < cir->input_cnt; i++)
    {
        int temp = get_id(cir->inputs[i]);
    }

    // assign a unique ID to every gate output
    for (int i = 0; i < cir->gate_cnt; i++)
    {
        int temp = get_id(cir->gates[i].output);
    }

    print_map();

    int clauses_cnt = 0;

    // FIXME:
    // temp write header at the top of file then seek later to overwrite
    // with correct clause count. this is bad tho and should fix by storing all
    // the clauses in a data structure and printing later
    fprintf(dimacs_fptr, "p cnf 00 00\n");

    // get the cnf form of every gate in the circuit
    for (int i = 0; i < cir->gate_cnt; i++)
    {
        // generate AND CNF
        if (cir->gates[i].type == AND)
        {
            int output_id = get_id(cir->gates[i].output);

            for (int j = 0; j < cir->gates[i].input_cnt; j++)
            {
// FIXME: for some reason ID 3 keeps getting changed to 13 here
                int input_id = get_id(cir->gates[i].inputs[j]);
                fprintf(dimacs_fptr, "%d -%d 0\n", input_id, output_id);
                clauses_cnt++;
            }

            fprintf(dimacs_fptr, "%d ", output_id);

            for (int j = 0; j < cir->gates[i].input_cnt; j++)
            {
                int input_id = get_id(cir->gates[i].inputs[j]);
                fprintf(dimacs_fptr, "-%d ", input_id);
            }

            fprintf(dimacs_fptr, "0\n");
            clauses_cnt++;
        }
        // generate OR CNF
        else if (cir->gates[i].type == OR)
        {
            int output_id = get_id(cir->gates[i].output);

            for (int j = 0; j < cir->gates[i].input_cnt; j++)
            {
                int input_id = get_id(cir->gates[i].inputs[j]);
                fprintf(dimacs_fptr, "-%d %d 0\n", input_id, output_id);
                clauses_cnt++;
            }

            fprintf(dimacs_fptr, "-%d ", output_id);

            for (int j = 0; j < cir->gates[i].input_cnt; j++)
            {
                int input_id = get_id(cir->gates[i].inputs[j]);
                fprintf(dimacs_fptr, "%d ", input_id);
            }

            fprintf(dimacs_fptr, "0\n");
            clauses_cnt++;
        }
        // genreate XOR
        else if (cir->gates[i].type == XOR)
        {
            int output_id = get_id(cir->gates[i].output);
            int input1_id = get_id(cir->gates[i].inputs[0]);
            int input2_id = get_id(cir->gates[i].inputs[1]);

            fprintf(dimacs_fptr, "%d %d -%d 0\n", input1_id, input2_id, output_id);
            fprintf(dimacs_fptr, "%d -%d %d 0\n", input1_id, input2_id, output_id);
            fprintf(dimacs_fptr, "-%d %d %d 0\n", input1_id, input2_id, output_id);
            fprintf(dimacs_fptr, "-%d -%d -%d 0\n", input1_id, input2_id, output_id);
            clauses_cnt += 4;
        }
    }

    // end with circuit output gate doing stuff
    fprintf(dimacs_fptr, "%d 0\n", get_id(cir->outputs[0]));
    clauses_cnt++;

    // temp fix for clause cnt
    fseek(dimacs_fptr, 0, SEEK_SET);
    fprintf(dimacs_fptr, "p cnf %d %d\n", id_cnt, clauses_cnt);
    fclose(dimacs_fptr);

    return 0;
}
