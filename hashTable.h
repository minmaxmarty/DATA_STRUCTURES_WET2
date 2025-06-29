#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "myExceptions.h"
#include <utility> // For std::move
#include <memory>  // For std::shared_ptr, std::make_shared, and std::enable_shared_from_this

// ------------------------------------- node (Hash Table Entry) ------------------------------------- //
template <typename K, typename D_ptr>
struct node {
    K m_key;
    D_ptr m_data;
    std::shared_ptr<node<K, D_ptr>> m_next;

    node(K key, D_ptr data, std::shared_ptr<node<K, D_ptr>> next = nullptr)
        : m_key(key), m_data(std::move(data)), m_next(std::move(next)) {}
};

// ----------------------------------- hashTable ----------------------------------- //
template <typename K, typename D_ptr>
class hashTable {
    const int INITIAL_SIZE = 17;
    const double RESIZE_FACTOR_UP = 2.0;
    const double RESIZE_FACTOR_DOWN = 0.5;

    std::shared_ptr<node<K, D_ptr>>* m_table;
    int m_bucket = INITIAL_SIZE;
    int m_size = 0;

    int hashFunction(const K& key) const;
    void resize(double factor);

public:
    hashTable();
    ~hashTable();
    std::shared_ptr<node<K, D_ptr>> insert(const K &key, D_ptr data);
    void remove(const K& key);
    std::shared_ptr<node<K, D_ptr>> find(const K& key) const;
};

template<typename K, typename D_ptr>
hashTable<K, D_ptr>::hashTable() {
    m_table = new std::shared_ptr<node<K, D_ptr>>[m_bucket]();
}

template<typename K, typename D_ptr>
hashTable<K, D_ptr>::~hashTable() {
    delete[] m_table;
}

template<typename K, typename D_ptr>
int hashTable<K, D_ptr>::hashFunction(const K &key) const {
    // Ensure result is non-negative for the modulo operation
    return (key % m_bucket + m_bucket) % m_bucket;
}

template<typename K, typename D_ptr>
void hashTable<K, D_ptr>::resize(double factor) {
    int oldBucketSize = m_bucket;
    m_bucket = static_cast<int>(oldBucketSize * factor);
    if (m_bucket < INITIAL_SIZE) {
        m_bucket = INITIAL_SIZE;
    }
    if (m_bucket == oldBucketSize) return;

    auto newTable = new std::shared_ptr<node<K, D_ptr>>[m_bucket]();

    for (int i = 0; i < oldBucketSize; ++i) {
        auto cur = m_table[i];
        while (cur != nullptr) {
            auto next = cur->m_next;
            int newIndex = hashFunction(cur->m_key);
            cur->m_next = newTable[newIndex];
            newTable[newIndex] = cur;
            cur = next;
        }
    }
    delete[] m_table;
    m_table = newTable;
}

template<typename K, typename D_ptr>
std::shared_ptr<node<K, D_ptr>> hashTable<K, D_ptr>::insert(const K &key, D_ptr data) {
    if (m_size >= m_bucket) {
        resize(RESIZE_FACTOR_UP);
    }
    
    int index = hashFunction(key);
    auto newNode = std::make_shared<node<K, D_ptr>>(key, std::move(data), m_table[index]);
    m_table[index] = newNode;
    m_size++;
    return newNode;
}

template<typename K, typename D_ptr>
void hashTable<K, D_ptr>::remove(const K &key) {
    int index = hashFunction(key);
    auto cur = m_table[index];
    std::shared_ptr<node<K, D_ptr>> prev = nullptr;
    while (cur != nullptr) {
        if (cur->m_key == key) {
            if (prev != nullptr) {
                prev->m_next = cur->m_next;
            } else {
                m_table[index] = cur->m_next;
            }
            m_size--;
            if (m_size > INITIAL_SIZE && m_size <= m_bucket / 4) {
                resize(RESIZE_FACTOR_DOWN);
            }
            return;
        }
        prev = cur;
        cur = cur->m_next;
    }
    throw key_doesnt_exist();
}

template<typename K, typename D_ptr>
std::shared_ptr<node<K, D_ptr>> hashTable<K, D_ptr>::find(const K &key) const {
    int index = hashFunction(key);
    auto cur = m_table[index];
    while (cur != nullptr) {
        if (cur->m_key == key) return cur;
        cur = cur->m_next;
    }
    return nullptr;
}


// ------------------------------------- setNode (Disjoint-Set Data) ------------------------------------- //
template <typename D>
class setNode : public std::enable_shared_from_this<setNode<D>> {
    D m_data;
    std::shared_ptr<setNode> m_parent;
    int m_size = 0; // Genres start with 0 songs.
    int m_uniteCounter = 0;

public:
    explicit setNode(D data, int initial_counter = 0)
        : m_data(data), m_parent(nullptr), m_uniteCounter(initial_counter) {}

    std::shared_ptr<setNode> findRoot();
    static void unite(std::shared_ptr<setNode> child_root, std::shared_ptr<setNode> parent_root);
    int getUniteCounter() const { return m_uniteCounter; }
    std::shared_ptr<setNode> getParent() const { return m_parent; } 
    const D& getData() const { return m_data; }
    void addToSize(int size_to_add) { m_size += size_to_add; }
    int getSize() const { return m_size; }
    void compress(std::shared_ptr<setNode<D>> root, int depth);
};

template<typename D>
std::shared_ptr<setNode<D>> setNode<D>::findRoot() {
    int total_merges = 0;
    std::shared_ptr<setNode<D>> root = this->shared_from_this();
    while (root->m_parent != nullptr) {
        total_merges += root->m_uniteCounter;
        root = root->m_parent;
    }
    compress(root, total_merges);
    return root;
}
void setNode<D>::compress(std::shared_ptr<setNode<D>> root, int depth) {
    std::shared_ptr<setNode<D>> cur = this->shared_from_this();
    while (cur != root) {
        int oldDepth = cur->m_uniteCounter;
        cur->m_parent = root;
        cur->m_uniteCounter = depth;
        depth -= oldDepth;
        cur = cur->m_parent;
    }
}
template<typename D>
void setNode<D>::unite(std::shared_ptr<setNode<D>> child_root, std::shared_ptr<setNode<D>> parent_root) {
    parent_root->addToSize(child_root->getSize());
    child_root->m_parent = parent_root;
    child_root->m_uniteCounter = 1;
}

#endif //HASHTABLE_H