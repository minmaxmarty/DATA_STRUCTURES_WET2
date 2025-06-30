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


// In dspotify25b2.cpp
StatusType DSpotify::addSong(int songId, int genreId){
    if (genreId <= 0 || songId <= 0) return StatusType::INVALID_INPUT;

    auto genre_ht_node = m_genreHT.find(genreId);
    if (genre_ht_node == nullptr || m_songHT.find(songId) != nullptr) {
        return StatusType::FAILURE;
    }

    try {
        // 1. Find the true root of the genre's set.
        auto genreRoot = std::static_pointer_cast<genreNode<int>>(genre_ht_node->m_data->findRoot());

        auto newSongNode = std::make_shared<songNode<int>>(songId);
        m_songHT.insert(songId, newSongNode);

        // 2. Connect the song to the ROOT.
        newSongNode->setParent(genreRoot);
        
        // 3. The song starts with 1 genre. Its counter is relative to the root.
        newSongNode->setUniteCounter(1 - genreRoot->getUniteCounter());

        // 4. Increment the song count on the ROOT of the set.
        genreRoot->addToSize(1);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

// In dspotify25b2.cpp
StatusType DSpotify::mergeGenres(int genreId1, int genreId2, int genreId3){
    if (genreId1 <= 0 || genreId2 <= 0 || genreId3 <= 0 || genreId1 == genreId2 ||
        genreId1 == genreId3 || genreId2 == genreId3)
        return StatusType::INVALID_INPUT;

    auto genre1_ht_node = m_genreHT.find(genreId1);
    auto genre2_ht_node = m_genreHT.find(genreId2);

    if (genre1_ht_node == nullptr || genre2_ht_node == nullptr || m_genreHT.find(genreId3) != nullptr)
        return StatusType::FAILURE;

    try {
        // 1. Find the ROOTS of the genre sets.
        auto root1 = genre1_ht_node->m_data->findRoot();
        auto root2 = genre2_ht_node->m_data->findRoot();

        if (root1 == root2) return StatusType::SUCCESS; // Already merged

        // 2. Perform the efficient union on the roots.
        auto finalRoot = genreNode<int>::uniteBySize(root1, root2);
        
        // 3. Update the logical data of the new root.
        finalRoot->setData(genreId3);
        finalRoot->setUniteCounter(finalRoot->getUniteCounter() + 1);

        // 4. Update the hash table to reflect the merge.
        m_genreHT.remove(genreId1);
        m_genreHT.remove(genreId2);

        // genreId3 now points to the new, merged root.
        m_genreHT.insert(genreId3, std::static_pointer_cast<genreNode<int>>(finalRoot));

        // Re-create the old genres as new, empty sets as per the requirements.
        auto g1New = std::make_shared<genreNode<int>>(genreId1);
        auto g2New = std::make_shared<genreNode<int>>(genreId2);
        m_genreHT.insert(genreId1, g1New);
        m_genreHT.insert(genreId2, g2New);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

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
// In dspotify25b2.cpp
output_t<int> DSpotify::getNumberOfSongsByGenre(int genreId){
    if (genreId <= 0) return StatusType::INVALID_INPUT;
    auto genre = m_genreHT.find(genreId);
    if (genre == nullptr) return StatusType::FAILURE;

    // FIX: Find the root to get the correct size of the entire set.
    auto root = std::static_pointer_cast<genreNode<int>>(genre->m_data->findRoot());
    return output_t<int>(root->getNumberOfSongs());
}

output_t<int> DSpotify::getNumberOfGenreChanges(int songId){
    if (songId <= 0) return StatusType::INVALID_INPUT;
    const auto& song_node_ht = m_songHT.find(songId);
    if (song_node_ht == nullptr) return StatusType::FAILURE;

    auto& songSetNode = song_node_ht->m_data;
    
    // FIX: Calling findRoot() updates the song's counter via path compression.
    auto genreRoot = songSetNode->findRoot();

    // The total changes is the song's (now updated) counter + its final root's counter.
    return output_t<int>(songSetNode->getUniteCounter() + genreRoot->getUniteCounter());
}