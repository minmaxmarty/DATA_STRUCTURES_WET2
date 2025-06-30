#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "myExceptions.h"
#include <utility> // For std::move
#include <memory>  // For std::shared_ptr, std::make_shared, and std::enable_shared_from_this

using namespace std;
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
    const int INITIAL_SIZE = 97;
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
class setNode : public enable_shared_from_this<setNode<D>> {
    D m_data;

    shared_ptr<setNode<D>> m_parent;

    int m_uniteCounter = 0;
    int numberOfSongs = 0; // This is used for genres, songs will not use this field

public:
    int getNumberOfSongs() const { return numberOfSongs; }
    void setSize(int size) { numberOfSongs = size; }
    void addToSize(int size) { numberOfSongs += size; }

    explicit setNode(D data) : m_data(data) {}

    shared_ptr<setNode<D>> findRoot();
    void compressAndCalc(shared_ptr<setNode<D>> root, int counter, int rootUniteCounter);

    static shared_ptr<setNode<D>> uniteBySize(shared_ptr<setNode<D>> A, shared_ptr<setNode<D>> B);
    static shared_ptr<setNode<D>> unite(shared_ptr<setNode<D>> into, shared_ptr<setNode<D>> from);

    void setUniteCounter(int counter);

    const D& getData() const;
    int getUniteCounter() const;
    shared_ptr<setNode<D>> getParent() const;
    void setParent(shared_ptr<setNode<D>> parent);

    void setData(const D& data) { m_data = data; }
};

// In hashTable.h, inside the setNode class
template<typename D>
shared_ptr<setNode<D>> setNode<D>::findRoot() {
    // Base case: If this node has no parent, it is the root.
    if (m_parent == nullptr) {
        return this->shared_from_this();
    }

    // Recursive step: Find the ultimate root of the parent.
    shared_ptr<setNode<D>> ultimate_root = m_parent->findRoot();

    // --- Path Compression & Counter Update ---
    // This happens "on the way back" from the recursion.
    
    // My new counter should be the sum of the old path.
    // My counter (dist to parent) + parent's counter (dist from parent to root)
    m_uniteCounter += m_parent->getUniteCounter();

    // Point me directly to the ultimate root.
    m_parent = ultimate_root;

    return ultimate_root;
}


//TODO decide which uniteBySize to use
//TODO how we implement nodes of genres and songs, maybe need to go to genre node
// In hashTable.h, inside the setNode class
// This function assumes A and B are already roots, which we'll enforce in mergeGenres.
template<typename D>
static shared_ptr<setNode<D>> uniteBySize(shared_ptr<setNode<D>> rootA, shared_ptr<setNode<D>> rootB) {
    if (rootA == rootB) return rootA; // Already in the same set.

    // Cast to genreNode to access song count, which we use as our "size"
    auto rootA_genre = std::static_pointer_cast<genreNode<D>>(rootA);
    auto rootB_genre = std::static_pointer_cast<genreNode<D>>(rootB);

    // Ensure rootA is the larger set by song count
    if (rootA_genre->getNumberOfSongs() < rootB_genre->getNumberOfSongs()) {
        std::swap(rootA_genre, rootB_genre);
    }

    // Attach the smaller tree (rootB) under the larger one (rootA)
    rootB_genre->setParent(rootA_genre);
    rootA_genre->addToSize(rootB_genre->getNumberOfSongs());

    // Update the counter of the old root relative to the new one.
    rootB_genre->setUniteCounter(rootB_genre->getUniteCounter() - rootA_genre->getUniteCounter());

    return rootA_genre;
}

template<typename D>
void setNode<D>::setUniteCounter(int counter) {
    m_uniteCounter = counter;
}

template<typename D>
const D& setNode<D>::getData() const {
    return m_data;
}

template<typename D>
int setNode<D>::getUniteCounter() const {
    return m_uniteCounter;
}

template<typename D>
shared_ptr<setNode<D>> setNode<D>::getParent() const {
    return m_parent;
}

template<typename D>
void setNode<D>::setParent(shared_ptr<setNode<D>> parent) {
        m_parent = parent;
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
    int numberOfSongs = 0;
public:
    genreNode(const D &data) : setNode<D>(data) {}
    int getNumberOfSongs() const { return numberOfSongs; }
    void setSize(int size) { numberOfSongs = size; }
    void addToSize(int size) { numberOfSongs += size; }
};

