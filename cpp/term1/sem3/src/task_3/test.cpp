#include "node_utils.hpp"
#include "node_utils.hpp"

#include "../simple_test.h"


TEST(nodes, empty) {
    Node *head = nullptr;
    
    revert(&head);
    EXPECT_EQ(nullptr, head);

    cleanup(head);
}


TEST(nodes, single_element) {
    Node *element = new Node{42, nullptr};
    Node *head = element;
    
    revert(&head);
    EXPECT_EQ(element, head);
    EXPECT_EQ(42, head->value);
    EXPECT_EQ(nullptr, head->next);

    cleanup(head);
}


TEST(nodes, revert) {
    Node *initLast = new Node{66, nullptr};
    Node *initMiddle = new Node{24, initLast};
    Node *initHead = new Node{42, initMiddle};

    Node *head = initHead;
    EXPECT_EQ(42, head->value);
    EXPECT_EQ(24, head->next->value);
    EXPECT_EQ(66, head->next->next->value);
    EXPECT_EQ(nullptr, head->next->next->next);

    revert(&head);

    EXPECT_EQ(initLast, head);
    EXPECT_EQ(66, head->value);

    EXPECT_EQ(initMiddle, head->next);
    EXPECT_EQ(24, head->next->value);

    EXPECT_EQ(initHead, head->next->next);
    EXPECT_EQ(42, head->next->next->value);

    EXPECT_EQ(nullptr, head->next->next->next);

    cleanup(head);
}


TESTING_MAIN()
