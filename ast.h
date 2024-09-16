#ifndef AST_H
#define AST_H

struct Node {
    enum { NODE_SELECT, NODE_STAR, NODE_IDENTIFIER, NODE_NUMBER, NODE_CONDITION, NODE_LIST } type;
    union {
        struct {
            struct Node *select_list;
            struct Node *table_list;
            struct Node *where_clause;
        } select_node;
        char *identifier;
        int number;
        struct {
            struct Node *left;
            struct Node *right;
        } condition_node;
        struct {
            struct Node *node;
            struct Node *next;
        } list_node;
    } data;
};

struct Node *make_select_node(struct Node *select_list, struct Node *table_list, struct Node *where_clause);
struct Node *make_star_node();
struct Node *make_identifier_node(char *identifier);
struct Node *make_number_node(int number);
struct Node *make_condition_node(struct Node *left, struct Node *right);
struct Node *append_to_list(struct Node *list, struct Node *node);

// Update the print_ast declaration to include indentation level
void print_ast(struct Node *node, int indent_level);

extern char output_buffer[];
void append_to_output(const char *str);

#endif /* AST_H */
