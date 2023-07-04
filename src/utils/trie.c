#include <malloc.h>
#include "trie.h"
#include "err.h"
#include "vector.h"

trie_t *trie_new()
{
    trie_t *res = malloc(sizeof(trie_t));
    if (res == NULL)
        return NULL;

    res->prefix = -1;
    res->children = vector_new();
    if (res->children == NULL)
    {
        free(res);
        return NULL;
    }
    res->data = NULL;

    return res;
}

void trie_free(trie_t *tree)
{
    for (size_t i = 0; i < vector_len(tree->children); i++)
    {
        trie_free(vector_get(tree->children, i));
    }
    vector_free(tree->children);

    free(tree);
}

unsigned char get_prefix(trie_t *tree)
{
    return (unsigned char)tree->prefix & 0xFF;
}

int find_insert_index(trie_t *tree, unsigned char prefix, size_t *index)
{
    size_t start = 0;
    size_t end = vector_len(tree->children);
    while (start < end)
    {
        size_t middle = (end + start) / 2;
        trie_t *child = vector_get(tree->children, middle);
        unsigned char middle_prefix = get_prefix(child);
        if (middle_prefix == prefix)
        {
            *index = middle;
            return 0;
        }
        else if (prefix < middle_prefix)
        {
            end = middle;
        }
        else
        {
            start = middle + 1;
        }
    }
    *index = start;
    return -1;
}

trie_t *find_create_child(trie_t *tree, unsigned char prefix)
{
    size_t index;
    if (find_insert_index(tree, prefix, &index) == 0)
    {
        return vector_get(tree->children, index);
    }
    trie_t *child = trie_new();
    if (child == NULL)
    {
        errx(-1, "malloc error: Could not allocate memory for trie creation");
    }
    child->prefix = (short)(prefix | 0x0000);
    vector_insert(tree->children, index, child);
    return child;
}

trie_t *find_create_path(trie_t *root, unsigned long address)
{
    trie_t *current = root;
    unsigned long mask = 0xFF << ((ADDRESS_SIZE - 1) * 8);
    for (size_t i = 0; i < ADDRESS_SIZE; i++)
    {
        unsigned char prefix
            = (address & mask) >> ((ADDRESS_SIZE - 1 - i) * 8);
        current = find_create_child(current, prefix);
        mask >>= 8;
    }
    return current;
}

void *trie_get(trie_t *tree, unsigned long address)
{
    trie_t *child = find_create_path(tree, address);
    return child->data;
}

void trie_set(trie_t *tree, unsigned long address, void *data)
{
    trie_t *child = find_create_path(tree, address);
    child->data = data;
}

void trie_for_each_rec(trie_t *tree, unsigned long parent_key,
    trie_function_t cb, void *additionals)
{
    unsigned long key
        = (parent_key << 8) + ((unsigned char)tree->prefix & 0xFF);
    if (tree->data != NULL)
    {
        cb(tree->data, key, additionals);
    }
    for (size_t i = 0; i < vector_len(tree->children); i++)
    {
        trie_for_each_rec(vector_get(tree->children, i), key, cb, additionals);
    }
}

void trie_for_each(trie_t *tree, trie_function_t function, void *additionals)
{
    for (size_t i = 0; i < vector_len(tree->children); i++)
    {
        trie_for_each_rec(
            vector_get(tree->children, i), 0, function, additionals);
    }
}

unsigned long trie_last_address(trie_t *tree)
{
    unsigned long address = 0;
    while (
        tree->children != NULL
        && (tree = vector_get(tree->children, vector_len(tree->children) - 1))
               != NULL)
    {
        address = (address << 8) | get_prefix(tree);
    }
    return address;
}

unsigned long trie_first_address(trie_t *tree)
{
    unsigned long address = 0;
    while (tree->children != NULL
           && (tree = vector_get(tree->children, 0)) != NULL)
    {
        address = (address << 8) | get_prefix(tree);
    }
    return address;
}
