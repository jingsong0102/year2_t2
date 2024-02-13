/*!
@file ObjectAllocator.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2183
@section A
@assignent 1
@date 2/02/2024
@brief This file contains the definition of the ObjectAllocator class and its related classes and structures.
*/
#include "ObjectAllocator.h"
#include <cstring>
/**
 * Constructor for the ObjectAllocator class.
 *
 * @param ObjectSize The size of each object to be allocated.
 * @param config Configuration settings for memory allocation.
 */
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) : config_(config), stats_(), OAException_()
{
    FreeList_ = NULL;
    PageList_ = NULL;

    stats_.ObjectSize_ = ObjectSize;
    leftBlock = sizeof(GenericObject *) + config_.PadBytes_ + config_.HBlockInfo_.size_;
    InnerBlock = stats_.ObjectSize_ + 2 * config_.PadBytes_ + config_.HBlockInfo_.size_;
    Alignment();

    stats_.PageSize_ = sizeof(GenericObject *) + static_cast<size_t>(config.ObjectsPerPage_) * ObjectSize + config_.PadBytes_ * 2 * config_.ObjectsPerPage_ + config_.HBlockInfo_.size_ * (config_.ObjectsPerPage_) + config_.LeftAlignSize_ + (config_.ObjectsPerPage_ - 1) * config_.InterAlignSize_;

    leftBlock = leftBlock + config_.LeftAlignSize_;
    InnerBlock = InnerBlock + config_.InterAlignSize_;

    OAException_ = new OAException(OAException::E_BAD_BOUNDARY, "Error message");

    if (config_.UseCPPMemManager_)
        return;
    lowerBound = PageAllocator();
    upperBound = lowerBound + stats_.PageSize_;
}
/**
 * Allocates memory for a new page.
 *
 * @return Pointer to the allocated memory.
 * @throws OAException if memory allocation fails.
 */
char *ObjectAllocator::PageAllocator()
{
    char *newPage{};
    try
    {
        // update accounting info
        newPage = new char[stats_.PageSize_];
        GenericObject *temp = PageList_;
        PageList_ = reinterpret_cast<GenericObject *>(newPage);
        PageList_->Next = temp;
        temp = NULL;
        stats_.PagesInUse_ += 1;
        stats_.FreeObjects_ += config_.ObjectsPerPage_;

        // create free list
        GenericObject *currentObj = NULL;
        currentObj = FreeList_;
        char *NewObject = newPage + sizeof(GenericObject *) + config_.PadBytes_ + config_.HBlockInfo_.size_ + config_.LeftAlignSize_; // point after page's Next
        memset(NewObject - config_.PadBytes_ - config_.HBlockInfo_.size_, 0x00, config_.HBlockInfo_.size_);
        memset(NewObject - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
        memset(NewObject, UNALLOCATED_PATTERN, stats_.ObjectSize_);
        memset(NewObject + stats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
        memset(NewObject - config_.PadBytes_ - config_.HBlockInfo_.size_ - config_.LeftAlignSize_, ALIGN_PATTERN, config_.LeftAlignSize_);
        FreeList_ = reinterpret_cast<GenericObject *>(NewObject);
        FreeList_->Next = NULL;
        currentObj = NULL;
        NewObject = NULL;
        unsigned int countBlocks = config_.ObjectsPerPage_ - 1;
        while (countBlocks)
        {
            currentObj = FreeList_;
            NewObject = reinterpret_cast<char *>(FreeList_);
            NewObject = NewObject + stats_.ObjectSize_ + config_.PadBytes_ * 2 + config_.HBlockInfo_.size_ + config_.InterAlignSize_;
            memset(NewObject - config_.PadBytes_ - config_.HBlockInfo_.size_, 0x00, config_.HBlockInfo_.size_);
            memset(NewObject - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
            memset(NewObject, UNALLOCATED_PATTERN, stats_.ObjectSize_);
            memset(NewObject + stats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
            memset(NewObject - config_.PadBytes_ - config_.HBlockInfo_.size_ - config_.InterAlignSize_, ALIGN_PATTERN, config_.InterAlignSize_);
            FreeList_ = reinterpret_cast<GenericObject *>(NewObject);
            FreeList_->Next = currentObj;
            currentObj = NULL;
            NewObject = NULL;

            countBlocks--;
        }
        return newPage;
    }
    catch (std::bad_alloc &)
    {
        throw OAException(OAException::E_NO_MEMORY, "No system memory available");
    }
}
/**
 * Destructor for the ObjectAllocator class.
 */
ObjectAllocator::~ObjectAllocator()
{
    delete OAException_;
    if (config_.UseCPPMemManager_)
        return;
    GenericObject *current = PageList_;
    while (current)
    {
        PageList_ = current;
        current = PageList_->Next;
        delete[] PageList_;
    }
}
/**
 * Allocates memory for an object.
 *
 * @param label Label associated with the allocation (for debugging).
 * @return Pointer to the allocated memory.
 * @throws OAException if memory allocation fails or boundary checks fail.
 */
void *ObjectAllocator::Allocate(const char *label)
{
    if (config_.UseCPPMemManager_)
    {
        stats_.Allocations_ += 1;
        size_t size = stats_.ObjectSize_;

        if (stats_.Allocations_ > stats_.MostObjects_)
            stats_.MostObjects_ = stats_.Allocations_;

        return new char[size];
    }

    if (stats_.FreeObjects_ == 0 && stats_.PagesInUse_ < config_.MaxPages_)
    {
        char *newPageStart;
        newPageStart = PageAllocator(); // create another page and link to preivous page
        if (newPageStart < lowerBound)
            lowerBound = newPageStart;

        newPageStart += stats_.PageSize_;
        if (newPageStart > upperBound)
            upperBound = newPageStart;
    }
    char *newObj{};
    if (stats_.FreeObjects_ > 0)
    {
        newObj = reinterpret_cast<char *>(FreeList_);
        FreeList_ = FreeList_->Next;
        // update account info
        stats_.ObjectsInUse_ += 1;
        stats_.FreeObjects_ -= 1;
        stats_.Allocations_ += 1;
        if (stats_.Allocations_ > stats_.MostObjects_)
            stats_.MostObjects_ = stats_.Allocations_;

        // update header block info
        char *use_count = NULL;
        char *pAlloc = NULL;
        char *headerblock = newObj - config_.PadBytes_ - config_.HBlockInfo_.size_;
        if (config_.HBlockInfo_.type_ == OAConfig::hbExtended)
        {
            use_count = headerblock + config_.HBlockInfo_.additional_;
            pAlloc = headerblock + config_.HBlockInfo_.additional_ + 2;
            *use_count = static_cast<char>(*use_count + 1);
            memset(pAlloc, static_cast<int>(stats_.Allocations_), 1);
            memset(pAlloc + 4, 0x1, 1);
        }
        else if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            MemBlockInfo *memBlockInfo = NULL;
            try
            {
                memBlockInfo = new MemBlockInfo();
            }
            catch (std::bad_alloc &)
            {
                throw OAException(OAException::E_NO_MEMORY, std::string(" No system memory available"));
            }
            memBlockInfo->in_use = true;
            memBlockInfo->label = const_cast<char *>(label);
            memBlockInfo->alloc_num = stats_.Allocations_;
            *(reinterpret_cast<MemBlockInfo **>(headerblock)) = memBlockInfo;
        }
        else if (config_.HBlockInfo_.type_ == OAConfig::hbBasic)
        {
            pAlloc = headerblock;
            memset(pAlloc, static_cast<int>(stats_.Allocations_), 1);
            memset(pAlloc + 4, 0x1, 1);
        }
        memset(newObj, ALLOCATED_PATTERN, stats_.ObjectSize_);
        return reinterpret_cast<void *>(newObj);
    }
    else
    {
        throw OAException(OAException::E_NO_MEMORY, "allocate_new_page: No system memory available.");
    }
}
/**
 * Deallocates memory for an object.
 *
 * @param Object Pointer to the memory to be deallocated.
 * @throws OAException if double-free, bad boundary, or corrupted block is detected.
 */
void ObjectAllocator::Free(void *Object)
{
    if (config_.UseCPPMemManager_)
    {
        stats_.Deallocations_ += 1;
        delete[] reinterpret_cast<char *>(Object);
        return;
    }

    // check for double free
    char *pObj = reinterpret_cast<char *>(Object);
    bool doubleFree = !checkObjectPattern(pObj, FREED_PATTERN);

    // check for bad boundary
    GenericObject *page = PageList_;
    int ret = FindPage(page, Object);
    if (ret == -1)
        throw OAException(OAException::E_BAD_BOUNDARY, "Bad boundary");
    size_t ObjectPosition = static_cast<size_t>(reinterpret_cast<char *>(Object) - reinterpret_cast<char *>(page));
    bool isAligned = ((ObjectPosition - leftBlock) % InnerBlock) == 0;
    bool baseCase = ObjectPosition % stats_.PageSize_ == 0;
    if (!isAligned || baseCase)
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Bad boundary");
    }

    // check for out of bounds
    if (Object < lowerBound || Object > upperBound)
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Bad boundary");
    }

    // Validate padding
    if (config_.PadBytes_ != 0)
    {
        PadState ps = isPadCorrupted(Object);
        if (ps == CORRUPT_LEFT)
            throw OAException(OAException::E_CORRUPTED_BLOCK, "corruption on left");
        if (ps == CORRUPT_RIGHT)
            throw OAException(OAException::E_CORRUPTED_BLOCK, "corruption on right");
    }
    if (doubleFree)
    {
        GenericObject *temp = FreeList_;
        FreeList_ = reinterpret_cast<GenericObject *>(Object);
        FreeList_->Next = temp;
        temp = NULL;

        char *headerblock = reinterpret_cast<char *>(Object) - config_.PadBytes_ - config_.HBlockInfo_.size_;
        char *pAlloc = NULL;
        size_t BytesInBasicBlock;
        if (config_.HBlockInfo_.type_ == OAConfig::hbExtended)
        {
            pAlloc = headerblock + config_.HBlockInfo_.additional_ + 2;
            BytesInBasicBlock = config_.HBlockInfo_.size_ - config_.HBlockInfo_.additional_ - 2;
        }
        else
        {
            pAlloc = headerblock;
            BytesInBasicBlock = config_.HBlockInfo_.size_;
        }
        if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            delete[] *reinterpret_cast<MemBlockInfo **>(headerblock);
        }

        memset(pAlloc, 0x00, BytesInBasicBlock);
        char *ptrToFreedPatternArea = reinterpret_cast<char *>(FreeList_) + sizeof(GenericObject *);
        memset(ptrToFreedPatternArea, FREED_PATTERN, stats_.ObjectSize_ - sizeof(GenericObject *));

        // update acounting info
        stats_.ObjectsInUse_ -= 1;
        stats_.FreeObjects_ += 1;
        stats_.Deallocations_ += 1;
    }
    else if (!doubleFree)
    {
        throw OAException(OAException::E_MULTIPLE_FREE, "Double free");
    }
}
/**
 * Dumps memory blocks that are currently in use.
 *
 * @param fn Callback function for processing the dumped memory.
 * @return Number of memory blocks in use.
 */
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    unsigned leaks = 0;
    GenericObject *page = PageList_;
    while (page)
    {
        char *block = reinterpret_cast<char *>(page) + sizeof(size_t) + OAConfig::BASIC_HEADER_SIZE + config_.PadBytes_ + config_.LeftAlignSize_;
        char *headerblock;
        bool in_use = false;
        for (int i = 0; i < DEFAULT_OBJECTS_PER_PAGE; i++)
        {

            char *pFlag;
            in_use = false;
            headerblock = block - config_.PadBytes_ - config_.HBlockInfo_.size_;
            if (config_.HBlockInfo_.type_ == OAConfig::hbExtended)
            {
                pFlag = headerblock + 6 + config_.HBlockInfo_.additional_;
                if (*pFlag)
                    in_use = true;
            }
            else if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
            {
                in_use = reinterpret_cast<MemBlockInfo *>(headerblock)->in_use;
            }
            else if (config_.HBlockInfo_.type_ == OAConfig::hbBasic)
            {
                pFlag = headerblock + 4;
                if (*pFlag)
                    in_use = true;
            }
            else
            {

                pFlag = (block - 1);
                if (*pFlag)
                    in_use = true;
                pFlag = NULL;
            }
            if (in_use)
            {
                fn(block, stats_.ObjectSize_);
                leaks++;
                in_use = false;
            }
            block = block + stats_.ObjectSize_ + config_.PadBytes_ * 2 + config_.HBlockInfo_.size_;
        }
        page = page->Next;
    }
    return leaks;
}
/**
 * Validates the memory pages for corruption.
 *
 * @param fn Callback function for processing corrupted memory.
 * @return Number of corrupted memory blocks.
 */
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
    unsigned corrupts = 0;
    GenericObject *page = PageList_;
    while (page)
    {
        char *block = reinterpret_cast<char *>(page) + sizeof(size_t) + config_.PadBytes_ + config_.HBlockInfo_.size_;
        for (int i = 0; i < DEFAULT_OBJECTS_PER_PAGE; i++)
        {
            PadState padState = isPadCorrupted(reinterpret_cast<GenericObject *>(block));
            if (padState == CORRUPT_LEFT || padState == CORRUPT_RIGHT)
            {
                fn(block, stats_.ObjectSize_);
                corrupts++;
            }
            block = block + stats_.ObjectSize_ + config_.PadBytes_ * 2 + config_.HBlockInfo_.size_;
        }
        page = page->Next;
    }
    return corrupts;
}
/**
 * Frees empty memory pages.
 *
 * @return Number of freed memory pages.
 */
unsigned ObjectAllocator::FreeEmptyPages()
{
    unsigned freedPages = 0;
    GenericObject *pPage = PageList_;
    GenericObject *prev_page = pPage;
    GenericObject *page_dele{};
    size_t count = 0;

    while (pPage)
    {
        count++;
        // check if page is empty
        if (isPageEmpty(pPage))
        {
            prev_page->Next = pPage->Next;
            freedPages++;
            stats_.FreeObjects_ -= config_.ObjectsPerPage_;
            stats_.PagesInUse_--;
            page_dele = pPage;
        }

        prev_page = pPage;
        pPage = pPage->Next;
        if ((count == 1) && (page_dele))
        {
            PageList_ = pPage;
        }

        // delete header block info
        if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            MemBlockInfo *headerblock = reinterpret_cast<MemBlockInfo *>(reinterpret_cast<char *>(page_dele) + sizeof(GenericObject *) + config_.LeftAlignSize_);

            for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
            {
                MemBlockInfo *memBlockInfo = headerblock;
                headerblock = reinterpret_cast<MemBlockInfo *>(reinterpret_cast<char *>(headerblock) + InnerBlock);
                delete memBlockInfo->label;
                delete memBlockInfo;
            }
        }
        if (page_dele)
        {
            delete[] page_dele;
        }
        page_dele = NULL;
    }

    return freedPages;
}
/**
 * Checks if a memory page is empty.
 *
 * @param page Pointer to the memory page.
 * @return True if the page is empty, false otherwise.
 */
bool ObjectAllocator::isPageEmpty(void *page) const
{

    char *block = reinterpret_cast<char *>(page) + sizeof(GenericObject *) + config_.PadBytes_ + config_.HBlockInfo_.size_ + config_.LeftAlignSize_;
    size_t count = config_.ObjectsPerPage_;
    size_t freeObj = 0;
    while (count)
    {
        if (checkPattern(block, FREED_PATTERN))
        {
            freeObj++;
        }
        else if (checkPattern(block, UNALLOCATED_PATTERN))
        {
            freeObj++;
        }

        if (freeObj == config_.ObjectsPerPage_)
        {
            return true;
        }

        block = block + stats_.ObjectSize_ + 2 * config_.PadBytes_ + config_.HBlockInfo_.size_ + config_.InterAlignSize_;
        count--;
    }
    return false;
}
/**
 * Checks if a memory block has a specific pattern.
 *
 * @param block Pointer to the memory block.
 * @param pattern The pattern to check for.
 * @return True if the pattern is found, false otherwise.
 */
bool ObjectAllocator::checkPattern(void *block, const unsigned char pattern) const
{

    char *obj = reinterpret_cast<char *>(block) + sizeof(GenericObject *);
    size_t count = stats_.ObjectSize_ - sizeof(GenericObject *);
    size_t totalCheck = count;
    size_t totalChecked = 0;
    while (count)
    {
        if (static_cast<unsigned char>(*obj) == pattern)
            totalChecked++;
        count--;
    }
    if (totalChecked == totalCheck)
    {
        return true;
    }
    return false;
}
/**
 * Aligns memory blocks to a specific alignment.
 */
void ObjectAllocator::Alignment()
{
    if (config_.Alignment_ == 0)
        return;

    if (static_cast<unsigned int>(leftBlock) % config_.Alignment_ != 0)
        config_.LeftAlignSize_ = config_.Alignment_ - (static_cast<unsigned int>(leftBlock) % config_.Alignment_);
    else
        config_.LeftAlignSize_ = 0;

    if (static_cast<unsigned int>(InnerBlock) % config_.Alignment_ != 0)
        config_.InterAlignSize_ = config_.Alignment_ - (static_cast<unsigned int>(InnerBlock) % config_.Alignment_);
    else
        config_.InterAlignSize_ = 0;
}
/**
 * Checks if a memory block has a specific pattern.
 *
 * @param block Pointer to the memory block.
 * @param pattern The pattern to check for.
 * @return True if the pattern is found, false otherwise.
 */
bool ObjectAllocator::checkObjectPattern(void *block, const unsigned char pattern) const
{

    char *obj = reinterpret_cast<char *>(block) + sizeof(GenericObject *);

    size_t count = stats_.ObjectSize_ - sizeof(GenericObject *);
    size_t totalCheck = count;
    size_t totalChecked = 0;
    while (count)
    {
        if (static_cast<unsigned char>(*obj) == pattern)
            totalChecked++;
        count--;
    }
    if (totalChecked == totalCheck)
    {
        return true;
    }
    return false;
}
/**
 * Finds a memory page that contains a specific memory block.
 *
 * @param page_ Pointer to the memory page.
 * @param Object Pointer to the memory block.
 * @return 1 if the page is found, -1 otherwise.
 */
int ObjectAllocator::FindPage(GenericObject *&page_, void *Object) const
{
    GenericObject *page = reinterpret_cast<GenericObject *>(page_);
    size_t lower;
    size_t upper;
    while (page)
    {
        lower = reinterpret_cast<size_t>(Object) - reinterpret_cast<size_t>(page);
        upper = reinterpret_cast<size_t>(page) + stats_.PageSize_ - reinterpret_cast<size_t>(Object);

        if (lower + upper == stats_.PageSize_ && lower < stats_.PageSize_)
        {
            page_ = page;
            return 1;
        }
        page = page->Next;
    }
    return -1;
}
/**
 * Checks if the padding of a memory block is corrupted.
 *
 * @param block Pointer to the memory block.
 * @return The state of the padding.
 */
ObjectAllocator::PadState ObjectAllocator::isPadCorrupted(void *block) const
{
    if (config_.PadBytes_ != 0)
    {
        char *leftPad = reinterpret_cast<char *>(block) - config_.PadBytes_;
        char *rightPad = reinterpret_cast<char *>(block) + stats_.ObjectSize_;

        int count = config_.PadBytes_;
        while (count != 0)
        {
            if (static_cast<unsigned char>(*leftPad) != PAD_PATTERN)
            {
                return ObjectAllocator::CORRUPT_LEFT;
            }
            if (static_cast<unsigned char>(*rightPad) != PAD_PATTERN)
            {
                return ObjectAllocator::CORRUPT_RIGHT;
            }
            block = reinterpret_cast<char *>(block) + 1;
            count--;
        }
    }
    return ObjectAllocator::NO_CORRUPT;
}
/**
 * Sets the debug state of the memory allocator.
 *
 * @param State The debug state to set.
 */
void ObjectAllocator::SetDebugState(bool State)
{
    config_.DebugOn_ = State;
}
/**
 * Gets the debug state of the memory allocator.
 *
 * @return The debug state of the memory allocator.
 */
const void *ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}
/**
 * Gets the debug state of the memory allocator.
 *
 * @return The debug state of the memory allocator.
 */
const void *ObjectAllocator::GetPageList() const
{
    return PageList_;
}
/**
 * Gets the debug state of the memory allocator.
 *
 * @return The debug state of the memory allocator.
 */
OAConfig ObjectAllocator::GetConfig() const
{
    return config_;
}
/**
 * Gets the debug state of the memory allocator.
 *
 * @return The debug state of the memory allocator.
 */
OAStats ObjectAllocator::GetStats() const
{
    return stats_;
}
