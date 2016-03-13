#ifndef INCLUDED_TRIESET_H
#define INCLUDED_TRIESET_H

#include <cstring>
#include <iostream>

    // ==============
    // class TrieSet
    // ==============

template <typename KeyType, typename KeyIterType>
class TrieSet {

private:

    static const int NUM_CHILDREN = 16;

        // ===============
        // struct TrieNode
        // ===============

    struct TrieNode {
        KeyType        key;
        bool           hasKey;
        TrieNode      *children[NUM_CHILDREN]; // hex vals
        TrieNode      *parent;
        unsigned char  index;

        explicit TrieNode(unsigned char idx)
            : key()
            , hasKey(false)
            , parent(0)
            , index(idx)
        {
            std::cout << "Creating trie node index: " << (int)index << "\n";
            memset(children, 0, NUM_CHILDREN * sizeof(TrieNode*));
        }

        ~TrieNode()
        {
            std::cout << "Destroying trie node index: " << (int)index<< "\n";
            // print(std::cout);
            for (size_t i = 0; i < NUM_CHILDREN; ++i) {
                if (children[i]) delete children[i];
            }
        }

        bool isLeafNode() const
        {
            for (size_t i = 0; i < NUM_CHILDREN; ++i) {
                if (children[i]) return false;
            }

            return true;
        }

        std::ostream& print(std::ostream& o)
        {
            o << "["
              << " [ address: " << this << " ]"
              << " [ index: " << (int)index << " ]"
              << " [ key: " << key << " ]"
              << " [ hasKey: " << hasKey << " ]"
              << " [ parent: " << parent << " ]";

            for (size_t i = 0; i < NUM_CHILDREN; ++i) {
                o << " [ children : [ "
                  << i << " : " << children[i] << " ] ";
            }

            o << " ]]\n";

            return o;
        }
    };

    size_t     d_size;
    TrieNode  *d_roots[NUM_CHILDREN];

public:

        // ==============
        // class Iterator
        // ==============

    class Iterator {
    };

    TrieSet();
    ~TrieSet();

    void   insert(const KeyType& key);
    void   erase (const KeyType& key);
    void   clear ();
    size_t count (const KeyType& key) const;

    size_t size()  const { return d_size; }
    bool   empty() const { return d_size == 0; }

};


    // ======================
    // TrieSet Implementation
    // ======================

template <typename KeyType, typename KeyIterType>
TrieSet<KeyType, KeyIterType>::TrieSet()
    : d_size(0)
{
    memset(d_roots, 0, NUM_CHILDREN * sizeof(TrieNode*));
}

template <typename KeyType, typename KeyIterType>
TrieSet<KeyType, KeyIterType>::~TrieSet()
{
    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        if (d_roots[i]) delete(d_roots[i]);
    }
}

template <typename KeyType, typename KeyIterType>
void TrieSet<KeyType, KeyIterType>::insert(const KeyType& key)
{
    KeyIterType keyIter(key);

    TrieNode ** curChildren = 0;
    TrieNode * curParent = 0;
    TrieNode * curNode   = 0;

    curChildren = d_roots;

    for (; keyIter; ++keyIter) {
        size_t pos = keyIter.value();
        if (!curChildren[pos]) {
            curNode = new TrieNode(pos);
            curChildren[pos] = curNode;
            curNode->parent = curParent;
            curChildren = curNode->children;
            curParent = curNode;
        } else {
            curNode = curChildren[pos];
            curParent = curNode;
            curChildren = curNode->children;
        }
    }

    if (curNode && !curNode->hasKey) {
        curNode->key = key;
        curNode->hasKey = true;
        ++d_size;
    }
}

template <typename KeyType, typename KeyIterType>
size_t TrieSet<KeyType, KeyIterType>::count(const KeyType& key) const
{
    KeyIterType keyIter(key);

    TrieNode * const *curChildren = 0;
    TrieNode *  curNode = 0;

    curChildren = d_roots;

    for (; keyIter; ++keyIter) {
        size_t pos = keyIter.value();
        if (!curChildren[pos]) {
            return 0;
        }

        curNode = curChildren[pos];
        curChildren = curNode->children;
    }

    if (curNode) {
        return curNode->hasKey ? 1 : 0;
    }

    return 0;
}

template <typename KeyType, typename KeyIterType>
void TrieSet<KeyType, KeyIterType>::clear()
{
    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        if (d_roots[i]) delete d_roots[i];
        d_roots[i] = 0;
    }

    d_size = 0;
}

template <typename KeyType, typename KeyIterType>
void TrieSet<KeyType, KeyIterType>::erase(const KeyType& key)
{
    KeyIterType keyIter(key);

    TrieNode ** curChildren = 0;
    TrieNode *  curNode = 0;

    curChildren = d_roots;

    for (; keyIter; ++keyIter) {
        size_t pos = keyIter.value();
        if (!curChildren[pos]) {
            return;
        }

        curNode = curChildren[pos];
        curChildren = curNode->children;
    }

    if (!curNode || !curNode->hasKey) {
        return;
    }

    // There is actually something to erase.

    --d_size;

    // Case I: This is not a leaf node. Just remove the hasKey flag;

    if (!curNode->isLeafNode()) {
        curNode->hasKey = false;
        return;
    }

    // Case II: This is a leaf node. We should remove all nodes up
    // to the first parent that has hasKey flag or is not a leaf node

    TrieNode * curParent = 0;
    curChildren = 0;
    curNode->hasKey = false;
    while (curNode && curNode->isLeafNode() && !curNode->hasKey) {
        curParent = curNode->parent;
        curChildren = curParent ? curParent->children : d_roots;
        curChildren[curNode->index] = 0;
        delete curNode;
        curNode = curParent;
    }
}

#endif
