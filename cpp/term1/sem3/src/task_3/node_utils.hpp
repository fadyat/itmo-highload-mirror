#pragma once

struct Node {
    int value;
    Node *next;
};

void revert(Node **head);

void cleanup(Node *head);
