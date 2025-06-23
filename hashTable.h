//
// Created by areg1 on 6/22/2025.
//

#ifndef HASHTABLE_H
#define HASHTABLE_H

template <typename K, typename D>
class linkedList;

template <typename K, typename D>
class node {
    K m_key;
    D m_data;
    node* m_next;

public:
    node(K key, D data, node* next = nullptr) : m_key(key), m_data(data), m_next(next) {}
};

template <typename K, typename D>
class hashTable {
    constexpr int INITIAL_SIZE = 10;
    constexpr int RESIZE_FACTOR = 2;

    linkedList<K, D>** m_table;
    int m_bucket = INITIAL_SIZE;
    int m_size = 0;
    int hashFunction(const K& key) const;
    void resize();


public:
    hashTable();
    void insert(const K& key, const D& data);
    void remove(const K& key);
    node<K, D>* find(const K& key) const;

};

template <typename K, typename D>
class linkedList {
    friend hashTable<K, D>;
    node<K, D>* m_head = nullptr;
    node<K, D>* m_tail = nullptr;
    int m_size = 0;

public:
    linkedList(node<K, D>* head, node<K, D>* tail, int size) : m_head(head), m_tail(tail) , m_size(size) {}
    ~linkedList();
    node<K, D>* find(const K& key) const;
    void insert(const K& key, const D& data);
    void remove(const K& key);
};

// ----------------------------------- hashTable ----------------------------------- //

template<typename K, typename D>
hashTable<K, D>::hashTable() {
    m_table = new linkedList<K, D>*[m_bucket]();
}

template<typename K, typename D>
int hashTable<K, D>::hashFunction(const K &key) const {
    return key % m_bucket;
}

template<typename K, typename D>
void hashTable<K, D>::resize() {
    const int newBucketSize = m_size == m_bucket
                      ? m_bucket * RESIZE_FACTOR
                      : m_size == 0.25 * m_bucket
                            ? m_bucket / RESIZE_FACTOR
                            : -1;
    if (newBucketSize != -1) {
        auto newTable = new linkedList<K, D>*[newBucketSize];
        for (int i = 0; i < m_bucket; ++i) {
            newTable[i] = m_table[i];
        }
        delete[] m_table;
        m_table = newTable;
        m_bucket = newBucketSize;
    }
}

template<typename K, typename D>
void hashTable<K, D>::insert(const K &key, const D &data) {
    int index = hashFunction(key);
    if (m_table[index] == nullptr) {
        m_table[index] = new linkedList<K, D>();
    }
    m_table[index]->insert(key, data);
    m_size++;
}

template<typename K, typename D>
void hashTable<K, D>::remove(const K &key) {
    int index = hashFunction(key);
    if (m_table[index] != nullptr) {
        m_table[index]->remove(key);
        return;
    }
    // TODO: add doesn't exist exception
}

template<typename K, typename D>
node<K, D> * hashTable<K, D>::find(const K &key) const {
    const int index = hashFunction(key);
    return m_table[index]->find(key);
}

// ---------------------------------- linkedList ---------------------------------- //

template<typename K, typename D>
linkedList<K, D>::~linkedList() {
    node<K, D>* cur = m_head;
    node<K, D>* toDelete = nullptr;
    while (cur != nullptr) {
        toDelete = cur;
        cur = cur->m_next;
        delete toDelete;
    }
}

template<typename K, typename D>
void linkedList<K, D>::insert(const K &key, const D &data) {
    auto* newNode = new node<K, D>(key, data);
    if (m_head != nullptr) { // if the head exists
        m_tail->m_next = newNode;
    }
    else { // if head doesn't exist, add a new one
        m_head = newNode;
    }
    m_tail = newNode;
    m_size++;
}

template<typename K, typename D>
void linkedList<K, D>::remove(const K &key) {
    node<K, D>* prev = nullptr; // easy algo
    node<K, D>* cur = m_head;
    while (cur != nullptr) {
        if (cur->m_key == key) {
            if (prev != nullptr) {
                prev->m_next = cur->m_next;
            }
            else {
                m_head = cur->m_next;
            }
            cur->m_next = nullptr;
            delete cur;
        }
        prev = cur;
        cur = cur->m_next;
    }
    m_size--;
}

template<typename K, typename D>
node<K, D> * linkedList<K, D>::find(const K &key) const {
    node<K, D>* cur = m_head;
    while (cur != nullptr) {
        if (cur->m_key == key) {
            return cur;
        }
    }
    return nullptr;
}

#endif //HASHTABLE_H
