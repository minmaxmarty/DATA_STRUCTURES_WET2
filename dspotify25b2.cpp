// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template
#include <stdlib.h>
#include "dspotify25b2.h"


DSpotify::DSpotify()= default;

DSpotify::~DSpotify() = default;


// Find the genre by id, if it exists,
// Insert a new genre node into the genre hash table.( need to do hashing and insert into linked list)
// We have nodes in the linked list, and each one of the must point to the next one and also point to genreNode(it must be shared ptr)
StatusType DSpotify::addGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;
    if (m_genreHT.find(genreId) != nullptr) return StatusType::FAILURE;

    try {
        // Create the genre's genreNode. The constructor defaults to an initial_counter of 0.
        auto newGenreNode = std::make_shared<genreNode<int>>(genreId);
        newGenreNode->setParent(nullptr);
        m_genreHT.insert(genreId, newGenreNode);
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
        auto newSongNode = std::make_shared<songNode<int>>(songId);
        
        newSongNode->setParent(nullptr);
        m_songHT.insert(songId, newSongNode);

        // Find the true root of the genre.
        const auto& genreRoot = genre_node_in_ht->m_data;

        // Connect the song to the genre's root.
        newSongNode->setParent(genreRoot);
        newSongNode->setUniteCounter(1 - genreRoot->getUniteCounter()); // Initialize the unite counter to 1.

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

    auto genre1 = m_genreHT.find(genreId1);
    auto genre2 = m_genreHT.find(genreId2);
    auto genre3 = m_genreHT.find(genreId3);

    if (genre1 == nullptr || genre2 == nullptr || genre3 != nullptr)
        return StatusType::FAILURE;

    
    shared_ptr<genreNode<int>> g1New = nullptr;
    shared_ptr<genreNode<int>> g2New = nullptr;

    try {
        g1New = std::make_shared<genreNode<int>>(genreId1);
        g2New = std::make_shared<genreNode<int>>(genreId2);
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    const auto& finalroot = genreNode<int>::uniteBySize(genre2->m_data, genre1->m_data);

    finalroot->setUniteCounter(finalroot->getUniteCounter() + 1);

    if(finalroot->getData() == genreId1) {
       genre2->m_data->setUniteCounter(genre2->m_data->getUniteCounter() - genre1->m_data->getUniteCounter() + 1);
    } else {
       genre1->m_data->setUniteCounter(genre1->m_data->getUniteCounter() - genre2->m_data->getUniteCounter() + 1);
    }

    std::static_pointer_cast<genreNode<int>>(finalroot)->setSize(genre1->m_data->getNumberOfSongs() + genre2->m_data->getNumberOfSongs());
    finalroot->setData(genreId3);

    // Cast finalroot to shared_ptr<genreNode<int>> before inserting
    m_genreHT.insert(genreId3, std::static_pointer_cast<genreNode<int>>(finalroot));

    m_genreHT.remove(genreId1);
    m_genreHT.remove(genreId2);

    m_genreHT.insert(genreId1, g1New);
    m_genreHT.insert(genreId2, g2New);

    return StatusType::SUCCESS;
}

//Find root of the song
//Get the id of the root
output_t<int> DSpotify::getSongGenre(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;


    auto song = m_songHT.find(songId);
    if (song == nullptr) return StatusType::FAILURE;

    auto genre = song->m_data->findRoot();

    output_t<int> result(genre->getData());

    return result;
}

//Find genreNode by genreId
//Return the number of songs in the genreNode
output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;


    auto genre = m_genreHT.find(genreId);

    if (genre == nullptr) return StatusType::FAILURE;

    const int toReturn = genre->m_data->getNumberOfSongs();

    output_t<int> result(toReturn);

    return result;
}

//Find songNode by songId
//Find Root of the songNode(in find uniteCounter must update)
//Return the uniteCounter of the songNode
output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
    if( songId <= 0) return StatusType::INVALID_INPUT;

    const auto& song_node = m_songHT.find(songId);

    if (song_node == nullptr) return StatusType::FAILURE;

    auto& songSetNode = song_node->m_data;

    try{
        auto genreSetNode = songSetNode->findRoot();
        output_t<int> result(genreSetNode-> getUniteCounter() + songSetNode->getUniteCounter());
        // The findRoot call will update the songSetNode's parent and uniteCounter.
        // So we can return the uniteCounter directly.
        return result;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}
