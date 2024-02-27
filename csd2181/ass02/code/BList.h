/*!
@file BList.h
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 2
@date 2/17/2024
@brief This file contains the declaration of the BList class and its related classes and structures.
*/
////////////////////////////////////////////////////////////////////////////////
#ifndef BLIST_H
#define BLIST_H
////////////////////////////////////////////////////////////////////////////////

#include <string> // error strings

/*!
  The exception class for BList
*/
class BListException : public std::exception
{
  private:
    int m_ErrCode;             //!< One of E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR
    std::string m_Description; //!< Description of the exception

  public:
    /*!
      Constructor

      \param ErrCode
        The error code for the exception.

      \param Description
        The description of the exception.
    */
    BListException(int ErrCode, const std::string& Description) :
    m_ErrCode(ErrCode), m_Description(Description) {};

    /*!
      Get the kind of exception

      \return
        One of E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR
    */
    virtual int code() const {
      return m_ErrCode;
    }

    /*!
      Get the human-readable text for the exception

      \return
        The description of the exception
    */
    virtual const char *what() const throw() {
      return m_Description.c_str();
    }

    /*!
      Destructor is "implemented" because it needs to be virtual
    */
    virtual ~BListException() {
    }

    //! The reason for the exception
    enum BLIST_EXCEPTION {E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR};
};

/*!
  Statistics about the BList
*/
struct BListStats
{
    //!< Default constructor
  BListStats() : NodeSize(0), NodeCount(0), ArraySize(0), ItemCount(0)  {};

  /*! 
    Non-default constructor

    \param nsize
      Size of the node

    \param ncount
      Number of nodes in the list

    \param asize
      Number of elements in each node (array)

    \param count
      Number of items in the list

  */
  BListStats(size_t nsize, int ncount, int asize, int count) : 
  NodeSize(nsize), NodeCount(ncount), ArraySize(asize), ItemCount(count)  {};

  size_t NodeSize; //!< Size of a node (via sizeof)
  int NodeCount;   //!< Number of nodes in the list
  int ArraySize;   //!< Max number of items in each node
  int ItemCount;   //!< Number of items in the entire list
};  

/*!
  The BList class
*/
template <typename T, unsigned Size = 1>
class BList
{
 
  public:
    /*!
      Node struct for the BList
    */
    struct BNode
    {
      BNode *next;    //!< pointer to next BNode
      BNode *prev;    //!< pointer to previous BNode
      unsigned int count;      //!< number of items currently in the node
      T values[Size]; //!< array of items in the node

      //!< Default constructor
      BNode() : next(0), prev(0), count(0) {}
    };

    BList();                            // default constructor
    BList(const BList &rhs);            // copy constructor
    ~BList();                           // destructor
    BList& operator=(const BList &rhs); // assign operator

      // arrays will be unsorted, if calling either of these
    void push_back(const T& value);
    void push_front(const T& value);

      // arrays will be sorted, if calling this
    void insert(const T& value);

    void remove(int index);
    void remove_by_value(const T& value);

    int find(const T& value) const;       // returns index, -1 if not found

    T& operator[](int index);             // for l-values
    const T& operator[](int index) const; // for r-values

    size_t size() const;   // total number of items (not nodes)
    void clear();          // delete all nodes

    static size_t nodesize(); // so the allocator knows the size

      // For debugging
    const BNode *GetHead() const;
    BListStats GetStats() const;

  private:
    BNode *head_; //!< points to the first node
    BNode *tail_; //!< points to the last node

    // Other private data and methods you may need ...
    BListStats stats_;  //!< statistics about the BList
    /**
    * @brief Inserts an element into the specified node at the specified index.
    * 
    * @tparam T Type of elements stored in the BList.
    * @tparam Size Maximum number of elements in each node.
    * @param node Pointer to the node to insert the element into.
    * @param value The value to be inserted into the node.
    * @param index The index to insert the value at.
    */
    void insertNode(BNode *node, const T& value, int index);
    /**
     * @brief Copies the elements from one array to another.
     * 
     * @tparam T Type of elements stored in the BList.
     * @tparam Size Maximum number of elements in each node.
     * @param src Pointer to the source array.
     * @param dest Pointer to the destination array.
     * @param count Number of elements to copy.
     */
    void copyList(T* src, T* dest, const unsigned int count);
    /**
    * @brief Splits the specified node into two nodes.
    * 
    * @tparam T Type of elements stored in the BList.
    * @tparam Size Maximum number of elements in each node.
    * @param node Pointer to the node to be split.
    */
    void splitNode(BNode *node);
};

#include "BList.cpp"

#endif // BLIST_H
