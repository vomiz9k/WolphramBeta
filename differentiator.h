//#ifndef ALGEBRAH

//#define ALGEBRAH

#define AFTER_ZERO_ACCURACY "%0.2lf"




struct tree_function
{
    char name[10];
    double (*ptr)(double);
    char derr[100];
};


enum branch_types
{
    TREE_INTEGER,
    TREE_OPERATOR,
    TREE_FUNCTION,
    TREE_VAR
};


struct tree_branch
{
    void* data;
    char type;
    tree_branch** next;
    int kids;
};


int search_similar_add(tree_branch* branch);

tree_branch* derr_of_div(tree_branch* branch, char variable);
tree_branch* derr_of_mul(tree_branch* branch, char variable);
tree_branch* derr_of_frac(tree_branch* branch, char variable);
tree_branch* derr_of_add(tree_branch* branch, char variable);
tree_branch* derr_of_pow(tree_branch* branch, char variable);

tree_branch* branch_cpy(tree_branch* branch);
tree_branch* diffirintiate_tree(tree_branch* branch, char variable);
tree_branch* derivative_of_func(tree_branch* func_branch, char variable);
tree_branch* change_y_for_branch(tree_branch* change_in, tree_branch* tree_func, tree_branch* tree_derr);
tree_branch* uv_deffirintiate(tree_branch* change_in, tree_branch* u_value, tree_branch* v_value, char variable);

tree_branch* get_G(char* str);
tree_branch* get_E();
tree_branch* get_T();
tree_branch* get_C();
tree_branch* get_P();
tree_branch* get_N();
void get_calc_string(char* str);

void add_kid(tree_branch* branch);
tree_branch* new_branch_construct();

double get_neutral_el(char operation);

double value_count(double value, double difference, char operation);
double tree_count(tree_branch* branch, double* var_vals);

int make_tree_easier(tree_branch* branch);


void branch_clr(tree_branch* branch);
int branch_full_clr(tree_branch* branch);

void tree_print(tree_branch* head, char* fout_name);
void recursive_tree_print (tree_branch* branch, FILE* fout);


int search_same(tree_branch* search_into, tree_branch* what_to_search);
int is_same(tree_branch* branch_1, tree_branch* branch_2);



tree_branch* make_char_branch(tree_branch* branch, char value, int branch_type);
tree_branch* make_value_branch(tree_branch* branch, double value);


int print_domain(tree_branch* branch, FILE* latex);
int get_priority(tree_branch* branch);
int tree_txt(tree_branch* branch, FILE * fout);
int latex_maker(tree_branch* func, char variable, int count, double* var_vals);

int copy_branch_values(tree_branch* value, tree_branch* dest_branch);

int working_with_func(char* str);

tree_branch* normalize_tree(tree_branch* branch);
//#endif
