// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template
#include <stdlib.h>
#include "dspotify25b2.h"


DSpotify::DSpotify()= default;

DSpotify::~DSpotify() = default;


// The destructor can be removed as smart pointers handle memory automatically.
// In dspotify25b2.cpp

StatusType DSpotify::addGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;
    if (m_genreHT.find(genreId) != nullptr) return StatusType::FAILURE;

    try {
        // Create the genre's setNode. The constructor defaults to an initial_counter of 0.
        auto newGenreSetNode = std::make_shared<setNode<int>>(genreId);
        m_genreHT.insert(genreId, newGenreSetNode);
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}


StatusType DSpotify::addSong(int songId, int genreId){
    if (genreId <= 0 || songId <= 0) return StatusType::INVALID_INPUT;

    auto genre_node_in_ht = m_genreHT.find(genreId);
    if (genre_node_in_ht == nullptr || m_songHT.find(songId) != nullptr) {
        return StatusType::FAILURE;
    }

    try {
        // *** KEY CHANGE HERE ***
        // Create the song's setNode, explicitly giving it an initial counter of 1.
        auto newSongSetNode = std::make_shared<setNode<int>>(songId, 1);
        m_songHT.insert(songId, newSongSetNode);

        // Find the true root of the genre.
       const auto& genreRoot = genre_node_in_ht->m_data;

        // Connect the song to the genre's root.
        newSongSetNode->m_parent = genreRoot;

        // Increment the genre's song count by exactly 1.
        genreRoot->addToSize(1);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

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
    } catch (const std::bad_alloc&) {
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

    auto song = m_songHT.find(songId);
    if (song == nullptr) return StatusType::FAILURE;

    auto genre = song->m_data->findRoot();

    output_t result(genre->getData());

    return result;
}

output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;

    auto genre = m_genreHT.find(genreId);

    if (genre == nullptr) return StatusType::FAILURE;

    auto genreSetNode = genre->m_data;

    const int toReturn = genreSetNode->getParent() == genreSetNode ? genreSetNode->getSize() : 0;

    output_t result(toReturn);

    return result;
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
  if( songId <= 0) return StatusType::INVALID_INPUT;
const auto& song_node = m_songHT.find(songId);
    if (song_node == nullptr) return StatusType::FAILURE;
    auto& songSetNode = song_node->m_data;
try{
    songSetNode->findRoot();
    output_t<int> result(songSetNode-> getUniteCounter());
    // The findRoot call will update the songSetNode's parent and uniteCounter.
    // So we can return the uniteCounter directly.
    return result;
} catch (const std::bad_alloc&) {
    return StatusType::ALLOCATION_ERROR;
}
}
