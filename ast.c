#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

char output_buffer[4096];

void append_to_output(const char *str) {
    strcat(output_buffer, str);
}

void append_indent(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        append_to_output("    ");  // 4 spaces per indentation level
    }
}

struct Node *make_select_node(struct Node *select_list, struct Node *table_list, struct Node *where_clause) {
    struct Node *node = malloc(sizeof(struct Node));
    node->type = NODE_SELECT;
    node->data.select_node.select_list = select_list;
    node->data.select_node.table_list = table_list;
    node->data.select_node.where_clause = where_clause;
    return node;
}

struct Node *make_star_node() {
    struct Node *node = malloc(sizeof(struct Node));
    node->type = NODE_STAR;
    return node;
}

struct Node *make_identifier_node(char *identifier) {
    struct Node *node = malloc(sizeof(struct Node));
    node->type = NODE_IDENTIFIER;
    node->data.identifier = identifier;
    return node;
}

struct Node *make_number_node(int number) {
    struct Node *node = malloc(sizeof(struct Node));
    node->type = NODE_NUMBER;
    node->data.number = number;
    return node;
}

struct Node *make_condition_node(struct Node *left, struct Node *right) {
    struct Node *node = malloc(sizeof(struct Node));
    node->type = NODE_CONDITION;
    node->data.condition_node.left = left;
    node->data.condition_node.right = right;
    return node;
}

struct Node *append_to_list(struct Node *list, struct Node *node) {
    struct Node *current = list;
    if (current->type != NODE_LIST) {
        struct Node *list_node = malloc(sizeof(struct Node));
        list_node->type = NODE_LIST;
        list_node->data.list_node.node = current;
        list_node->data.list_node.next = NULL;
        current = list_node;
    }
    struct Node *last = current;
    while (last->data.list_node.next != NULL) {
        last = last->data.list_node.next;
    }
    struct Node *new_list_node = malloc(sizeof(struct Node));
    new_list_node->type = NODE_LIST;
    new_list_node->data.list_node.node = node;
    new_list_node->data.list_node.next = NULL;
    last->data.list_node.next = new_list_node;
    return current;
}

// Update print_ast to include indent_level
void print_ast(struct Node *node, int indent_level) {
    char temp[256];
    if (!node) return;
    switch (node->type) {
        case NODE_SELECT:
            append_indent(indent_level);
            append_to_output("SELECT statement\n");
            append_indent(indent_level);
            append_to_output("Select list:\n");
            print_ast(node->data.select_node.select_list, indent_level + 1);
            append_indent(indent_level);
            append_to_output("From tables:\n");
            print_ast(node->data.select_node.table_list, indent_level + 1);
            if (node->data.select_node.where_clause) {
                append_indent(indent_level);
                append_to_output("Where clause:\n");
                print_ast(node->data.select_node.where_clause, indent_level + 1);
            }
            break;
        case NODE_STAR:
            append_indent(indent_level);
            append_to_output("*\n");
            break;
        case NODE_IDENTIFIER:
            append_indent(indent_level);
            snprintf(temp, sizeof(temp), "Identifier: %s\n", node->data.identifier);
            append_to_output(temp);
            break;
        case NODE_NUMBER:
            append_indent(indent_level);
            snprintf(temp, sizeof(temp), "Number: %d\n", node->data.number);
            append_to_output(temp);
            break;
        case NODE_CONDITION:
            append_indent(indent_level);
            append_to_output("Condition:\n");
            print_ast(node->data.condition_node.left, indent_level + 1);
            append_indent(indent_level + 1);
            append_to_output("=\n");
            print_ast(node->data.condition_node.right, indent_level + 1);
            break;
        case NODE_LIST:
            print_ast(node->data.list_node.node, indent_level);
            if (node->data.list_node.next) {
                print_ast(node->data.list_node.next, indent_level);
            }
            break;
    }
}
