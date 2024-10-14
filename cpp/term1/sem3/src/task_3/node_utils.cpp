#include "node_utils.hpp"

void revert(Node **head) {
    Node *prev = nullptr, *current = *head, *next = nullptr;

    while (current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *head = prev;
}

void cleanup(Node *head) {
    Node *current = head, *next = nullptr;

    while (current != nullptr) {
        next = current->next;
        delete current;
        current = next;
    }
}
