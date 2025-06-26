//
// Created by areg1 on 6/22/2025.
//


#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "myExceptions.h"

template <typename K, typename D>
class linkedList;

// ------------------------------------- pair ------------------------------------- //


template<class A, class B>
class pair {
    A m_object_1;
    B m_object_2;

public:
    pair() = default;
    pair(const A& first, const B& second) : m_object_1(first), m_object_2(second) {}
    pair(const pair& other) : m_object_1(other.first()), m_object_2(other.second()) {}
    pair& operator=(const pair& other) = default;
    void setFirst(const A& data) { m_object_1 = data; }
    void setSecond(const B& data) { m_object_2 = data; }
    const A& first() const { return m_object_1; }
    const B& second() const { return m_object_2; }
};

// ------------------------------------- node ------------------------------------- //


//TODO I think there isn`t next and prev because  it can be some children
template <typename K, typename D>
struct node {
    K m_key;
    D m_data;
    node* m_next;
    node* m_prev;

    node(K key, D data, node* next = nullptr, node* prev = nullptr) : m_key(key), m_data(data), m_next(next), m_prev(prev) {}
};

// ----------------------------------- hashTable ----------------------------------- //

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
    ~hashTable();

    node<K, D> *insert(const K &key, const D &data);
    void remove(const K& key);
    node<K, D>* find(const K& key) const;

};

template<typename K, typename D>
hashTable<K, D>::hashTable() {
    m_table = new linkedList<K, D>*[m_bucket]();
}

template<typename K, typename D>
hashTable<K, D>::~hashTable() {
    for (int i = 0; i < m_bucket; ++i) {
        delete m_table[i];
    }
    delete[] m_table;
}

template<typename K, typename D>
int hashTable<K, D>::hashFunction(const K &key) const {
    return key % m_bucket;
}

template<typename K, typename D>
void hashTable<K, D>::resize() {
    const int newBucketSize = m_size >= m_bucket
                      ? m_bucket * RESIZE_FACTOR
                      : (m_size <= 0.25 * m_bucket && m_bucket > INITIAL_SIZE)
                            ? m_bucket / RESIZE_FACTOR
                            : -1;
    if (newBucketSize != -1) {
        auto newTable = new linkedList<K, D>*[newBucketSize]();
        const int oldBucketSize = m_bucket;
        m_bucket = newBucketSize;
        for (int i = 0; i < oldBucketSize; ++i) {
            while (m_table[i] != nullptr && m_table[i]->m_size != 0) {
                pair<K, D> toMove = m_table[i]->pop();
                int index = hashFunction(toMove.first());
                if (newTable[index] == nullptr) {
                    newTable[index] = new linkedList<K, D>();
                }
                newTable[index]->insert(toMove.first(), toMove.second());
            }
            delete m_table[i];
        }
        delete[] m_table;
        m_table = newTable;
    }
}

template<typename K, typename D>
node<K, D> *hashTable<K, D>::insert(const K &key, const D &data) {
    int index = hashFunction(key);
    if (m_table[index] == nullptr) {
        m_table[index] = new linkedList<K, D>();
    }
    node<K, D>* newNode = m_table[index]->insert(key, data);
    m_size++;
    resize();

    return newNode;
}

template<typename K, typename D>
void hashTable<K, D>::remove(const K &key) {
    int index = hashFunction(key);
    try {
        if (m_table[index] != nullptr) {
            m_table[index]->remove(key);
            if (m_table[index]->m_size == 0) {
                delete m_table[index];
                m_table[index] = nullptr;
            }
            m_size--;
            resize();
            return;
        }
    } catch (const key_doesnt_exist&) {
        throw key_doesnt_exist();
    }
    throw key_doesnt_exist();
}

template<typename K, typename D>
node<K, D> * hashTable<K, D>::find(const K &key) const {
    const int index = hashFunction(key);
    if (m_table[index] == nullptr) return nullptr;
    return m_table[index]->find(key);
}

// ---------------------------------- linkedList ---------------------------------- //

template <typename K, typename D>
class linkedList {
    friend hashTable<K, D>;
    node<K, D>* m_head;
    node<K, D>* m_tail;
    int m_size;

public:
    linkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {};
    linkedList(node<K, D>* head, node<K, D>* tail, int size) : m_head(head), m_tail(tail) , m_size(size) {}
    ~linkedList();
    node<K, D>* find(const K& key) const;

    node<K, D>* insert(const K &key, const D &data);
    void remove(const K& key);

    pair<K, D> pop();
};

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
node<K, D>* linkedList<K, D>::insert(const K &key, const D &data) {
    auto* newNode = new node<K, D>(key, data);
    if (m_head != nullptr) { // if the head exists
        m_tail->m_next = newNode;
        newNode->m_prev = m_tail;
    }
    else { // if head doesn't exist, add a new one
        m_head = newNode;
    }
    m_tail = newNode;
    m_size++;

    return newNode;
}

template<typename K, typename D>
void linkedList<K, D>::remove(const K &key) {
    node<K, D>* cur = m_head;
    while (cur != nullptr) {
        if (cur->m_key == key) {
            node<K, D>* prev = cur->m_prev;
            node<K, D>* next = cur->m_next;

            if (prev != nullptr) prev->m_next = next;
            if (next != nullptr) next->m_prev = prev;

            if (cur == m_head) m_head = next;
            if (cur == m_tail) m_tail = prev;

            delete cur;
            m_size--;
            return;
        }
        cur = cur->m_next;
    }
    if (cur == nullptr) throw key_doesnt_exist();
}


//TODO sonething strange that remove and pop have different return values
//TODO why i have pair and node, why i don't use pair in node?
template<typename K, typename D>
pair<K, D> linkedList<K, D>::pop() {
    if (m_size == 0) {
        throw out_of_range();
    }
    node<K, D>* node_to_remove = m_tail;
    pair<K, D> result(node_to_remove->m_key, node_to_remove->m_data);

    m_tail = node_to_remove->m_prev;
    if (m_tail == nullptr) {
        m_head = nullptr;
    } else {
        m_tail->m_next = nullptr;
    }
    //TODO check it; it is useless
    //node_to_remove->m_prev = nullptr;

    delete node_to_remove;
    m_size--;

    return result;
}

template<typename K, typename D>
node<K, D> * linkedList<K, D>::find(const K &key) const {
    node<K, D>* cur = m_head;
    while (cur != nullptr) {
        if (cur->m_key == key) {
            return cur;
        }
        cur = cur->m_next;
    }
    return nullptr;
}

// ------------------------------------- setNode ------------------------------------- //

template <typename D>
class tableNode {
    D m_data;
    tableNode* m_parent;

    int m_uniteCounter = 1;

    
    //TODO why I need nextAlloc? I think it is the same as bookKeeper
    // setNode* m_nextAlloc = nullptr;

    // int m_size = 1;
public:
    explicit tableNode(D data) : m_data(data), m_parent(this) {}

    tableNode* findRoot();
    void compress(tableNode *root);

    static tableNode* uniteBySize(tableNode* A, tableNode* B);
    static tableNode* unite(tableNode* into, tableNode* from);
   
    void incUniteCounter();

    const D& getData() const;
    int getUniteCounter() const;
    tableNode* getParent() const;

     // setNode* getNextAlloc() const;
    // void setNextAlloc(setNode* bookKeeper);
    // int getSize() const;
};

template<typename D>
tableNode<D> * tableNode<D>::findRoot() {
    tableNode* cur = this;
    int counter = 0; // TODO: add counter to count height from song to genre
    while (cur != cur->m_parent) {
        cur = cur->m_parent;
        counter++;
    }
    tableNode* root = cur;
    compress(root);
    return root;
}

template<typename D>
void tableNode<D>::compress(tableNode *root) {
    tableNode* cur = this;
    tableNode* next = nullptr;
    while (cur != cur->m_parent) {
        next = cur->m_parent;
        cur->m_parent = root;
        cur = next;
    }
}


// template<typename D>
// setNode<D> * setNode<D>::uniteBySize(setNode *A, setNode *B) {
//     aRoot = A->findRoot();
//     bRoot = B->findRoot();

//     if (aRoot->m_size < bRoot->m_size) {
//         setNode* temp = aRoot;
//         aRoot = bRoot;
//         bRoot = temp;
//     }

//     bRoot->m_parent = aRoot;
//     aRoot->m_size += bRoot->m_size;

//     return aRoot;
// }


//TODO decide which uniteBySize to use
//TODO how we implement nodes of genres and songs, maybe need to go to genre node
template<typename D>
tableNode<D> * tableNode<D>::uniteBySize(tableNode *A, tableNode *B) {
    tableNode* aRoot = A->findRoot();
    tableNode* bRoot = B->findRoot();

    if (aRoot->m_size < bRoot->m_size) {
        unite(bRoot, aRoot);

        return bRoot;
    } else {
        unite(aRoot, bRoot);

        return aRoot;
    }
}

template<typename D>
tableNode<D> * tableNode<D>::unite(tableNode *into, tableNode *from) {
    tableNode* intoRoot = into->findRoot();
    tableNode* fromRoot = from->findRoot();

    fromRoot->m_parent = intoRoot;
    intoRoot->m_size += fromRoot->m_si
    intoRoot->m_uniteCounter += fromRoot->m_uniteCounter;

    return intoRoot;
}

// template<typename D>
// setNode<D> * setNode<D>::getNextAlloc() const {
//     return m_nextAlloc;
// }

// template<typename D>
// void setNode<D>::setNextAlloc(setNode *bookKeeper) {
//     m_nextAlloc = bookKeeper;
// }

template<typename D>
void tableNode<D>::incUniteCounter() {
    m_uniteCounter++;
}

template<typename D>
const D & tableNode<D>::getData() const {
    return m_data;
}

// template<typename D>
// int setNode<D>::getSize() const {
//     return m_size;
// }

template<typename D>
int tableNode<D>::getUniteCounter() const {
    return m_uniteCounter;
}

template<typename D>
tableNode<D> * tableNode<D>::getParent() const {
    return m_parent;
}

#endif //HASHTABLE_H


// ------------------------------------- songNode ------------------------------------- //

template<typename D>
class songNode : public tableNode<D> {
public:
    songNode(const D &data) : tableNode<D>(data) {}
};

// ------------------------------------- genreNode ------------------------------------- //

template<typename D>
class genreNode : public tableNode<D> {
    
    int numberOfSongs = 1;
public:
    genreNode(const D &data) : tableNode<D>(data) {}
    int getSize() const { return numberOfSongs; }
};