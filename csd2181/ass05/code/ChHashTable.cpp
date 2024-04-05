/*!
@file ChHashTable.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignment 5
@date 4/5/2024
@brief This file contains the definition of the ChHashTable class and its related classes and structures.
*/
#include "ChHashTable.h"
/**
 * @brief Constructor for ChHashTable class.
 *
 * Constructs a ChHashTable object with the given configuration and allocator.
 *
 * @param Config The configuration for the hash table.
 * @param allocator An optional allocator for memory management.
 */
template <typename T>
ChHashTable<T>::ChHashTable(const HTConfig &Config, ObjectAllocator *allocator) : config_(Config), allocator_(allocator)
{
    table_ = new ChHTHeadNode[config_.InitialTableSize_];
    stats_ = new HTStats();
    stats_->TableSize_ = config_.InitialTableSize_;
    stats_->HashFunc_ = config_.HashFunc_;
    stats_->Allocator_ = allocator;
}
/**
 * @brief Destructor for ChHashTable class.
 *
 * Destructs the ChHashTable object and frees allocated memory.
 */
template <typename T>
ChHashTable<T>::~ChHashTable()
{
    clear();
    delete[] table_;
    delete stats_;
}
/**
 * @brief Inserts an element into the hash table.
 *
 * Inserts the specified element with the given key into the hash table.
 *
 * @param Key The key associated with the data to be inserted.
 * @param Data The data to be inserted into the hash table.
 * @throws HashTableException if the insertion fails due to a duplicate key or memory allocation failure.
 */
template <typename T>
void ChHashTable<T>::insert(const char *Key, const T &Data)
{
    try
    {
        double loadFactor = static_cast<double>((stats_->Count_ + 1)) / static_cast<double>(stats_->TableSize_);
        if (loadFactor > config_.MaxLoadFactor_)
        {
            grow_table();
        }
        unsigned index = config_.HashFunc_(Key, stats_->TableSize_);
        ChHTHeadNode *head = &table_[index];
        ChHTNode *node = head->Nodes;
        stats_->Probes_++;
        while (node)
        {
            stats_->Probes_++;
            if (strncmp(node->Key, Key, MAX_KEYLEN) == 0)
            {
                throw HashTableException(HashTableException::E_DUPLICATE, "Duplicate key");
            }
            node = node->Next;
        }
        ChHTNode *newNode = new_node(Data);
        strncpy(newNode->Key, Key, MAX_KEYLEN);
        newNode->Next = head->Nodes;
        head->Nodes = newNode;
        head->Count++;
        stats_->Count_++;
    }
    catch (HashTableException &e)
    {
        throw e;
    }
}
/**
 * @brief Removes an element from the hash table.
 *
 * Removes the element with the specified key from the hash table.
 *
 * @param Key The key of the element to be removed.
 */
template <typename T>
void ChHashTable<T>::remove(const char *Key)
{
    unsigned index = stats_->HashFunc_(Key, stats_->TableSize_);
    ChHTHeadNode *head = &table_[index];
    ChHTNode *node = head->Nodes;
    ChHTNode *prev = nullptr;
    while (node)
    {
        stats_->Probes_++;
        if (strncmp(node->Key, Key, MAX_KEYLEN) == 0)
        {
            if (prev)
            {
                prev->Next = node->Next;
            }
            else
            {
                head->Nodes = node->Next;
            }
            delete_node(node);
            head->Count--;
            stats_->Count_--;
            return;
        }
        prev = node;
        node = node->Next;
    }
}
/**
 * @brief Finds an element in the hash table.
 *
 * Finds and returns the element with the specified key from the hash table.
 *
 * @param Key The key of the element to be found.
 * @return The reference to the found element.
 * @throws HashTableException if the element with the specified key is not found.
 */
template <typename T>
const T &ChHashTable<T>::find(const char *Key) const
{
    unsigned index = stats_->HashFunc_(Key, stats_->TableSize_);
    ChHTNode *node = table_[index].Nodes;
    while (node)
    {
        if (strncmp(node->Key, Key, MAX_KEYLEN) == 0)
        {
            stats_->Probes_++;
            return node->Data;
        }
        node = node->Next;
        stats_->Probes_++;
    }
    throw HashTableException(HashTableException::E_ITEM_NOT_FOUND, "Item not found");
}
/**
 * @brief Clears the hash table.
 *
 * Removes all elements from the hash table.
 */
template <typename T>
void ChHashTable<T>::clear()
{
    for (unsigned i = 0; i < stats_->TableSize_; i++)
    {
        ChHTNode *node = table_[i].Nodes;
        while (node)
        {
            ChHTNode *temp = node;
            delete_node(node);
            node = temp->Next;
        }
        table_[i].Nodes = nullptr;
    }
    stats_->Count_ = 0;
}
/**
 * @brief Gets the statistics of the hash table.
 *
 * Returns the statistics of the hash table.
 *
 * @return The statistics of the hash table.
 */
template <typename T>
HTStats ChHashTable<T>::GetStats() const
{
    return *stats_;
}
/**
 * @brief Gets the table of the hash table.
 *
 * Returns the underlying table of the hash table.
 *
 * @return The table of the hash table.
 */
template <typename T>
const typename ChHashTable<T>::ChHTHeadNode *ChHashTable<T>::GetTable() const
{
    return table_;
}
/**
 * @brief Creates a new node with the given data.
 *
 * Creates a new node with the specified data.
 *
 * @param Data The data to be stored in the new node.
 * @return Pointer to the newly created node.
 * @throws HashTableException if memory allocation fails.
 */
template <typename T>
typename ChHashTable<T>::ChHTNode *ChHashTable<T>::new_node(const T &Data)
{
    try
    {
        if (allocator_)
        {
            return new (allocator_->Allocate()) ChHTNode(Data);
        }
        else
        {
            return new ChHTNode(Data);
        }
    }
    catch (const std::bad_alloc &e)
    {
        throw HashTableException(HashTableException::E_NO_MEMORY, "No memory");
    }
}
/**
 * @brief Deletes the specified node.
 *
 * Deletes the specified node and frees memory.
 *
 * @param node Pointer to the node to be deleted.
 */
template <typename T>
void ChHashTable<T>::delete_node(ChHTNode *&node)
{
    if (config_.FreeProc_)
    {
        config_.FreeProc_(node->Data);
    }
    if (allocator_)
    {
        allocator_->Free(node);
    }
    else
    {
        delete node;
    }
}
/**
 * @brief Grows the hash table.
 *
 * Increases the size of the hash table and rehashes all elements.
 *
 * @throws HashTableException if memory allocation fails.
 */
template <typename T>
void ChHashTable<T>::grow_table()
{
    try
    {
        unsigned oldSize = stats_->TableSize_;
        unsigned newSize = GetClosestPrime(static_cast<unsigned>(std::ceil(oldSize * config_.GrowthFactor_)));
        stats_->TableSize_ = newSize;
        ChHTHeadNode *newTable = new ChHTHeadNode[stats_->TableSize_];
        for (unsigned i = 0; i < oldSize; i++)
        {
            ChHTNode *node = table_[i].Nodes;
            while (node)
            {
                stats_->Probes_++;
                ChHTNode *temp = node->Next;
                unsigned index = stats_->HashFunc_(node->Key, stats_->TableSize_);
                if (newTable[index].Nodes)
                {
                    ChHTNode *head = newTable[index].Nodes;
                    while (head)
                    {
                        stats_->Probes_++;
                        if (strncmp(head->Key, node->Key, MAX_KEYLEN) == 0)
                        {
                            break;
                        }
                        head = head->Next;
                    }
                }
                node->Next = newTable[index].Nodes;
                newTable[index].Nodes = node;
                node = temp;
            }
        }
        delete[] table_;
        table_ = newTable;
        stats_->Expansions_++;
    }
    catch (const std::bad_alloc &e)
    {
        throw HashTableException(HashTableException::E_NO_MEMORY, "No memory");
    }
}