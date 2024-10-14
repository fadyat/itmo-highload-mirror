#include "network.hpp"

#include <cstring>
#include <cstdint>
#include <algorithm>

namespace {
    static std::uint64_t get_current_time() {
        static std::uint64_t current_time = 0;
        return current_time++;
    }
}

Post *create_post(char const *user, char const *text, Post *head) {
    char *user_copy = new char[strlen(user) + 1];
    std::copy(user, user + strlen(user) + 1, user_copy);

    char *text_copy = new char[strlen(text) + 1];
    std::copy(text, text + strlen(text) + 1, text_copy);

    Post *post = new Post{user_copy, text_copy, head, get_current_time(), 0};
    return post;
}

namespace {
    void clean_post(Post const *post) {
        delete[] post->user;
        delete[] post->text;
        delete post;
    }
}


void destroy_posts(Post const *head) {
    while (head) {
        Post *next = head->prev;
        clean_post(head);
        head = next;
    }
}

Post *previous(Post const *post) { return post->prev; }

char const *get_user(Post const *post) { return post->user; }

char const *get_text(Post const *post) { return post->text; }

std::uint64_t timestamp(Post const *post) { return post->created_at; }

void like_post(Post *post) { post->likes++; }

size_t get_likes(Post *post) { return post->likes; }

namespace {
    Post *reverse(Post *head) {
        Post *new_head = nullptr;

        while (head) {
            Post *next = head->prev;
            head->prev = new_head;
            new_head = head;
            head = next;
        }

        return new_head;
    }
}

Post *remove_by_content(Post *head, char const *content) {
    Post *new_head = nullptr;

    while (head) {
        Post *next = previous(head);

        if (strstr(get_text(head), content) == nullptr) {
            head->prev = new_head;
            new_head = head;
        } else {
            clean_post(head);
        }

        head = next;
    }

    return reverse(new_head);
}

Post *merge(Post *head_1, Post *head_2) {
    Post *new_head = nullptr;
    Post **current = &new_head;

    while (head_1 && head_2) {
        if (timestamp(head_1) > timestamp(head_2)) {
            *current = head_1;
            head_1 = previous(head_1);
        } else {
            *current = head_2;
            head_2 = previous(head_2);
        }

        current = &(*current)->prev;
    }

    *current = head_1 ? head_1 : head_2;
    return new_head;
}

void set_text(Post *post, char const *new_text) {
    delete[] post->text;
    post->text = new char[strlen(new_text) + 1];
    std::copy(new_text, new_text + strlen(new_text) + 1, const_cast<char *>(post->text));
}

namespace {
    void replace_with_stars_single(Post *post, char const *pattern) {
        if (!post) return;

        char *text_copy = new char[strlen(get_text(post)) + 1];
        std::copy(get_text(post), get_text(post) + strlen(get_text(post)) + 1, text_copy);

        for (char *pos = text_copy; (pos = strstr(pos, pattern)); ++pos) {
            std::fill(pos, pos + strlen(pattern), '*');
            pos += strlen(pattern) - 1;
        }

        set_text(post, text_copy);
        delete[] text_copy;
    }
}

void replace_with_stars(Post *post, char const *pattern) {
    while (post) {
        replace_with_stars_single(post, pattern);
        post = previous(post);
    }
}
