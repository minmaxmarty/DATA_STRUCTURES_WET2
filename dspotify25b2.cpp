// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"


DSpotify::DSpotify() : m_bookKeeper(nullptr) {}

// go in two hashes, genre and song, go one by one in table[i] and delete all nodes
// we need destructor to delete linked lists and nodes in them
DSpotify::~DSpotify() {
    setNode<int>* cur = m_bookKeeper;
    setNode<int>* next = nullptr;
    while (cur != nullptr) {
        next = cur->getNextAlloc();
        delete cur;
        cur = next;
    }
}

// Find the genre by id, if it exists,
// Insert a new genre node into the genre hash table.( need to do hashing and insert into linked list)
// We have nodes in the linked list, and each one of the must point to the next one and also point to genreNode(it must be shared ptr)
StatusType DSpotify::addGenre(int genreId){
    if (genreId <=0) return StatusType::INVALID_INPUT;
    if (m_genreHT.find(genreId) != nullptr) return StatusType::FAILURE;
    setNode<int>* newGenreSetNode = nullptr;
    try {
        newGenreSetNode = new setNode(genreId);
        m_genreHT.insert(genreId, newGenreSetNode);
    } catch (const std::bad_alloc&) {
        delete newGenreSetNode;
        return StatusType::ALLOCATION_ERROR;
    }

    newGenreSetNode->setNextAlloc(m_bookKeeper);
    m_bookKeeper = newGenreSetNode;

    return StatusType::SUCCESS;
}


//Insert a new song node into the song hash table.
// Find the genre by id, if it exists, and insert the song node into the genre
StatusType DSpotify::addSong(int songId, int genreId){
    if (genreId <= 0 || songId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* genre = m_genreHT.find(genreId);
    node<int, setNode<int>*>* song;

    if (genre == nullptr || m_songHT.find(songId) != nullptr) return StatusType::FAILURE;
    setNode<int>* newSongSetNode = nullptr;
    try {
        newSongSetNode = new setNode(songId);
        song = m_songHT.insert(songId, newSongSetNode);
    } catch (const std::bad_alloc&) {
        delete newSongSetNode;
        return StatusType::ALLOCATION_ERROR;
    }

    newSongSetNode->setNextAlloc(m_bookKeeper);
    m_bookKeeper = newSongSetNode;

    setNode<int>* genreSetNode = genre->m_data;
    setNode<int>* songSetNode = song->m_data;

    setNode<int>::uniteBySize(genreSetNode, songSetNode);

    return StatusType::SUCCESS;
}

//Make unite  of two genres by their ids into a new genre with id genreId3(one by one)
//update genreId3 number of songs like sum of genreId1 and genreId2
//Update the genreId1 and genreId2 unite counter + 1 
StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3){
    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 || genreId1 == genreId2 ||
        genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;

    genreNode<int>* genre1 = m_genreHT.find(genreId1);
    genreNode<int>* genre2 = m_genreHT.find(genreId2);
    genreNode<int>* genre3 = m_genreHT.find(genreId3);


    if (!genre1 || !genre2 || genre3)
        return StatusType::FAILURE;

    shared_ptr<genreNode<int>> newGenreNode = nullptr;
    shared_ptr<genreNode<int>> g1New = nullptr;
    shared_ptr<genreNode<int>> g2New = nullptr;

    try {
        newGenreNode = make_shared<genreNode<int>>(genreId3);
        g1New = std::make_shared<genreNode<int>>(genreId1);
        g2New = std::make_shared<genreNode<int>>(genreId2);
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    genreNode<int>::unite(newGenreNode.get(), genre1);
    genre1->setUniteCounter(genre1->getUniteCounter() + 1);

    genreNode<int>::unite(newGenreNode.get(), genre2);
    genre2->setUniteCounter(genre2->getUniteCounter() + 1);

    newGenreNode->setSize(genre1->getNumberOfSongs() + genre2->getNumberOfSongs());

    m_genreHT.remove(genreId1);
    m_genreHT.remove(genreId2);


    m_genreHT.insert(genreId3, newGenreNode);    

    m_genreHT.insert(genreId1, g1New);
    m_genreHT.insert(genreId2, g2New);

    return StatusType::SUCCESS;
}

//Find root of the song
//Get the id of the root
output_t<int> DSpotify::getSongGenre(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;

    songNode<int>* song = m_songHT.find(songId);
    if (!song) return StatusType::FAILURE;

    auto genre = song->findRoot();

    output_t result(genre->getData());

    return result;
}

//Find genreNode by genreId
//Return the number of songs in the genreNode
output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;

    genreNode<int>* genre = m_genreHT.find(genreId);

    if (!genre) return StatusType::FAILURE;

    const int toReturn = genre->getNumberOfSongs();

    output_t result(toReturn);

    return result;
}

//Find songNode by songId
//Find Root of the songNode(in find uniteCounter must update)
//Return the uniteCounter of the songNode
output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* song = m_songHT.find(songId);
    if (song == nullptr) return StatusType::FAILURE;

    setNode<int>* genre = song->m_data->findRoot();

    output_t result(genre->getUniteCounter());

    return result;
}






// go in two hashes, genre and song, go one by one in table[i] and delete all nodes
// we need destructor to delete linked lists and nodes in them
DSpotify::~DSpotify() {
    
}

// Find the genre by id, if it exists,
// Insert a new genre node into the genre hash table.( need to do hashing and insert into linked list)
// We have nodes in the linked list, and each one of the must point to the next one and also point to genreNode(it must be shared ptr)
StatusType DSpotify::addGenre(int genreId){
    
}


//Insert a new song node into the song hash table.
// Find the genre by id, if it exists, and insert the song node into the genre
StatusType DSpotify::addSong(int songId, int genreId){
    
}

//Make unite  of two genres by their ids into a new genre with id genreId3(one by one)
//update genreId3 number of songs like sum of genreId1 and genreId2
//Update the genreId1 and genreId2 unite counter + 1 
StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3){
    
}

//Find root of the song
//Get the id of the root
output_t<int> DSpotify::getSongGenre(int songId){
   
}

//Find genreNode by genreId
//Return the number of songs in the genreNode
output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
   
}

//Find songNode by songId
//Find Root of the songNode(in find uniteCounter must update)
//Return the uniteCounter of the songNode
output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
   
}
