/*!
@file ALGraph.h
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 4
@date 3/20/2024
@brief This file contains the declaration of the ALGraph class and its related classes and structures.
*/
//---------------------------------------------------------------------------
#ifndef ALGRAPH_H
#define ALGRAPH_H
//---------------------------------------------------------------------------
#include <vector>

struct DijkstraInfo
{
  unsigned cost;
  std::vector<unsigned> path;
};

struct AdjacencyInfo
{
  unsigned id;
  unsigned weight;
};

typedef std::vector<std::vector<AdjacencyInfo>> ALIST;

class ALGraph
{
public:
  /**
   * @brief Constructor.
   * @param size The size of the graph.
   */
  ALGraph(unsigned size);
  /**
   * @brief Destructor.
   */
  ~ALGraph(void);
  /**
   * @brief Adds a directed edge to the graph.
   * @param source The source node.
   * @param destination The destination node.
   * @param weight The weight of the edge.
   */
  void AddDEdge(unsigned source, unsigned destination, unsigned weight);
  /**
   * @brief Adds an undirected edge to the graph.
   * @param node1 The first node.
   * @param node2 The second node.
   * @param weight The weight of the edge.
   */
  void AddUEdge(unsigned node1, unsigned node2, unsigned weight);
  /**
   * @brief Performs Dijkstra's algorithm on the graph.
   * @param start_node The starting node for Dijkstra's algorithm.
   * @return A vector containing Dijkstra's algorithm information for each node.
   */
  std::vector<DijkstraInfo> Dijkstra(unsigned start_node) const;
  /**
   * @brief Gets the adjacency list representation of the graph.
   * @return The adjacency list representation.
   */
  ALIST GetAList(void) const;

private:
  // An EXAMPLE of some other classes you may want to create and
  // implement in ALGraph.cpp

  struct AdjInfo
  {
    unsigned int des;
    unsigned int cost;
    /**
     * @brief Default constructor.
     */
    AdjInfo();
    /**
     * @brief Parameterized constructor.
     * @param dst Destination node.
     * @param weight Weight of the edge.
     */
    AdjInfo(unsigned dst, unsigned weight);
    /**
     * @brief Less than operator for comparing AdjInfo objects.
     * @param rhs The right-hand side object.
     * @return True if this object is less than the right-hand side object, false otherwise.
     */
    bool operator<(const AdjInfo &rhs) const;
    /**
     * @brief Greater than operator for comparing AdjInfo objects.
     * @param rhs The right-hand side object.
     * @return True if this object is greater than the right-hand side object, false otherwise.
     */
    bool operator>(const AdjInfo &rhs) const;
  };

  // Other private fields and methods
  const unsigned INFINITY_ = static_cast<unsigned>(-1);
  unsigned size_;
  std::vector<std::vector<AdjInfo>> adj_list_;
};

#endif
