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


// ------------------------------------- setNode ------------------------------------- //

template <typename D>
class setNode : public std::enable_shared_from_this<setNode<D>> {
    D m_data;

    std::shared_ptr<setNode> m_parent;

    int m_uniteCounter = 0;

public:

    explicit setNode(D data) : m_data(data), m_parent(this) {}

    setNode* findRoot();
    void compressAndCalc(setNode<D>* root, int counter);

    static setNode<D>* uniteBySize(setNode<D>* A, setNode<D>* B);
    static setNode<D>* unite(setNode<D>* into, setNode<D>* from);

    void setUniteCounter(int counter);

    const D& getData() const;
    int getUniteCounter() const;
    setNode<D>* getParent() const;
};

template<typename D>
setNode<D> * setNode<D>::findRoot() {
    setNode<D>* cur = this;
    int uniteNum = 0;
    while (cur != cur->m_parent) {
        uniteNum += cur->getUniteCounter();
        cur = cur->m_parent;
    }
    setNode<D>* root = cur;
    compressAndCalc(root, uniteNum);

    return root;
}
template<typename D>

void setNode<D>::compressAndCalc(setNode<D>* root, int uniteNum) {
    setNode<D>* cur = this;
    setNode<D>* next = nullptr;

    while (cur != cur->m_parent) {
        next = cur->m_parent;
        int oldUniteCounter = cur->getUniteCounter();

        cur->m_parent = root;
        cur->setUniteCounter(uniteNum);

        uniteNum -= oldUniteCounter;
        cur = next;
    }
}


//TODO decide which uniteBySize to use
//TODO how we implement nodes of genres and songs, maybe need to go to genre node
template<typename D>
setNode<D> * setNode<D>::uniteBySize(setNode<D> *A, setNode<D> *B) {
    setNode<D>* aRoot = A->findRoot();
    setNode<D>* bRoot = B->findRoot();

    if (aRoot->m_size < bRoot->m_size) {
        unite(bRoot, aRoot);

        return bRoot;
    } else {
        unite(aRoot, bRoot);

        return aRoot;
    }
}

template<typename D>
setNode<D> * setNode<D>::unite(setNode<D> *into, setNode<D> *from) {
    setNode<D>* intoRoot = into->findRoot();
    setNode<D>* fromRoot = from->findRoot();

    fromRoot->m_parent = intoRoot;

    return intoRoot;
}

template<typename D>
void setNode<D>::setUniteCounter(int counter) {
    m_uniteCounter = counter;
}

template<typename D>
const D & setNode<D>::getData() const {
    return m_data;
}

template<typename D>
int setNode<D>::getUniteCounter() const {
    return m_uniteCounter;
}

template<typename D>
setNode<D>* setNode<D>::getParent() const {
    return m_parent;
}

#endif //HASHTABLE_H
// ------------------------------------- songNode ------------------------------------- //

template<typename D>
class songNode : public setNode<D> {
public:
    songNode(const D &data) : setNode<D>(data) {}
};

// ------------------------------------- genreNode ------------------------------------- //

template<typename D>
class genreNode : public setNode<D> {
    int numberOfSongs = 1;
public:
    genreNode(const D &data) : setNode<D>(data) {}
    int getNumberOfSongs() const { return numberOfSongs; }
    void setSize(int size) { numberOfSongs = size; }
};

