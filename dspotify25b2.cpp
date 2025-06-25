// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b2.h"


DSpotify::DSpotify() : m_bookKeeper(nullptr) {}

DSpotify::~DSpotify() {
    setNode<int>* cur = m_bookKeeper;
    setNode<int>* next = nullptr;
    while (cur != nullptr) {
        next = cur->getNextAlloc();
        delete cur;
        cur = next;
    }
}

StatusType DSpotify::addGenre(int genreId){
    if (genreId <=0) return StatusType::INVALID_INPUT;
    if (m_genreHT.find(genreId) != nullptr) return StatusType::FAILURE;
    setNode<int>* newGenreSetNode = nullptr;
    try {
        newGenreSetNode = new setNode(genreId);
        m_genreHT.insert(genreId, newGenreSetNode);
    } catch (std::bad_alloc) {
        delete newGenreSetNode;
        return StatusType::ALLOCATION_ERROR;
    }

    newGenreSetNode->setNextAlloc(m_bookKeeper);
    m_bookKeeper = newGenreSetNode;

    return StatusType::SUCCESS;
}

StatusType DSpotify::addSong(int songId, int genreId){
    if (genreId <= 0 || songId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* genre = m_genreHT.find(genreId);
    node<int, setNode<int>*>* song;

    if (genre == nullptr || m_songHT.find(songId) != nullptr) return StatusType::FAILURE;
    setNode<int>* newSongSetNode = nullptr;
    try {
        newSongSetNode = new setNode(songId);
        song = m_songHT.insert(songId, newSongSetNode);
    } catch (std::bad_alloc) {
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

StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3){
    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 || genreId1 == genreId2 ||
        genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* genre1 = m_genreHT.find(genreId1);
    node<int, setNode<int>*>* genre2 = m_genreHT.find(genreId2);

    if (genre1 == nullptr || genre2 == nullptr || m_genreHT.find(genreId3) != nullptr)
        return StatusType::FAILURE;

    setNode<int>* newGenreSetNode = nullptr;
    try {
        newGenreSetNode = new setNode(genreId3);
        m_genreHT.insert(genreId3, newGenreSetNode);
    } catch (std::bad_alloc) {
        delete newGenreSetNode;
        return StatusType::ALLOCATION_ERROR;
    }

    newGenreSetNode->setNextAlloc(m_bookKeeper);
    m_bookKeeper = newGenreSetNode;

    setNode<int>* genreSetNode1 = genre1->m_data;
    setNode<int>* genreSetNode2 = genre2->m_data;

    setNode<int>* unitedNode = setNode<int>::uniteBySize(genreSetNode1, genreSetNode2);
    setNode<int>* finalUnitedNode = setNode<int>::unite(newGenreSetNode, unitedNode);

    finalUnitedNode->incUniteCounter();

    return StatusType::SUCCESS;
}

output_t<int> DSpotify::getSongGenre(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* song = m_songHT.find(songId);
    if (song == nullptr) return StatusType::FAILURE;

    setNode<int>* genre = song->m_data->findRoot();

    output_t result(genre->getData());

    return result;
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* genre = m_genreHT.find(genreId);

    if (genre == nullptr) return StatusType::FAILURE;

    setNode<int>* genreSetNode = genre->m_data;

    const int toReturn = genreSetNode->getParent() == genreSetNode ? genreSetNode->getSize() : 0;

    output_t result(toReturn);

    return result;
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;

    node<int, setNode<int>*>* song = m_songHT.find(songId);
    if (song == nullptr) return StatusType::FAILURE;

    setNode<int>* genre = song->m_data->findRoot();

    output_t result(genre->getUniteCounter());

    return result;
}
