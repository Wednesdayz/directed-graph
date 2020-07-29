#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

namespace gdwg {
template <typename N, typename E>
class Graph {
 public:
  // Node
  struct Node {
    Node() : value_{} {}
    explicit Node(const N& value) : value_{value} {}
    N value_;
  };

  // Edge
  struct Edge {
    Edge(const std::shared_ptr<Node>& src, const std::shared_ptr<Node>& dst, const E& weight)
      : src_{src}, dst_{dst}, weight_{weight} {}
    std::weak_ptr<Node> src_;  // incoming node
    std::weak_ptr<Node> dst_;  // outgoing node
    E weight_;
  };

  struct NodeComparator {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
      return a->value_ < b->value_;
    }

    bool operator()(const std::shared_ptr<Node>& lhs, const N& val) const {
      return lhs->value_ < val;
    }

    bool operator()(const N& val, const std::shared_ptr<Node>& rhs) const {
      return val < rhs->value_;
    }
  };

  struct EdgeComparator {
    // compare src->dst->weight
    bool operator()(const std::shared_ptr<Edge>& a, const std::shared_ptr<Edge>& b) const {
      if (a->src_.lock()->value_ != b->src_.lock()->value_) {
        // since src_ is weak_ptr need to lock into shared_ptr to access value_
        return a->src_.lock()->value_ < b->src_.lock()->value_;
      } else if (a->dst_.lock()->value_ != b->dst_.lock()->value_) {
        return a->dst_.lock()->value_ < b->dst_.lock()->value_;
      } else {
        return a->weight_ < b->weight_;
      }
    }
  };

  // Iterator
  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = std::weak_ptr<std::tuple<N, N, E>>;
    using difference_type = int;

    reference operator*() const {
      return {edge_it_->get()->src_.lock()->value_, edge_it_->get()->dst_.lock()->value_,
              (*edge_it_).get()->weight_};
    }

    const_iterator& operator++() {
      ++edge_it_;
      return *this;
    }

    const_iterator& operator--() {
      --edge_it_;
      return *this;
    }

    const_iterator operator++(int) const {
      auto copy{*this};
      ++(*this);
      return copy;
    }

    pointer operator->() const { return &(operator*()); }

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      return ((lhs.edge_it_ == rhs.edge_it_) &&
              ((lhs.edge_it_ == lhs.sentinel_) ||
               (lhs.node1_it_ == rhs.node1_it_ && lhs.node2_it_ == rhs.node2_it_)));
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    typename std::set<std::shared_ptr<Node>, NodeComparator>::const_iterator node1_it_;
    typename std::set<std::shared_ptr<Node>, NodeComparator>::const_iterator node2_it_;
    typename std::set<std::shared_ptr<Edge>, EdgeComparator>::const_iterator edge_it_;
    typename std::set<std::shared_ptr<Edge>, EdgeComparator>::const_iterator sentinel_;
    friend class Graph;
    const_iterator(const decltype(node1_it_)& node1_it,
                   const decltype(node2_it_)& node2_it,
                   const decltype(edge_it_)& edge_it,
                   const decltype(sentinel_)& sentinel)
      : node1_it_{node1_it}, node2_it_{node2_it}, edge_it_{edge_it}, sentinel_{sentinel} {}
  };

  using iterator = const_iterator;
  using const_iterator = const_iterator;
  friend class const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /*
   * 2.1 Constructors
   */
  Graph() : nodes_{}, edges_{} {};
  Graph(typename std::vector<N>::const_iterator start, typename std::vector<N>::const_iterator end);
  Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator start,
        typename std::vector<std::tuple<N, N, E>>::const_iterator end);
  Graph(typename std::initializer_list<N> list);
  // Copy Construct
  Graph(const Graph<N, E>& g);
  // Move Constructor
  Graph(Graph<N, E>&& g);
  // Destructor
  ~Graph() = default;

  /*
   * 2.2 Operations
   */
  // Copy Assignment
  Graph<N, E>& operator=(const Graph<N, E>& g);
  // Move Assignment
  Graph<N, E>& operator=(Graph<N, E>&& g);

  /*
   *  2.3 Methods
   */
  bool InsertNode(const N& val) noexcept;
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& val) noexcept;
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear() noexcept;
  bool IsNode(const N& val) const noexcept;
  bool IsConnected(const N& src, const N& dst) const;
  std::vector<N> GetNodes() const noexcept;
  std::vector<N> GetConnected(const N& src) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;
  bool erase(const N& src, const N& dst, const E& w) noexcept;

  const_iterator find(const N& n1, const N& n2, const E& e);
  const_iterator erase(const_iterator it);
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
  const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }
  const_iterator begin();
  const_iterator end();
  const_reverse_iterator rbegin() { return crbegin(); }
  const_reverse_iterator rend() { return crend(); }

  /*
   * 2.4 Friends
   */
  friend bool operator==(const Graph<N, E>& g1, const Graph<N, E>& g2) {
    if (!(g1.nodes_.size() == g2.nodes_.size() && g1.edges_.size() == g2.edges_.size()))
      return false;

    const_iterator it1 = g1.cbegin();
    const_iterator it2 = g2.cbegin();

    while (it1 != g1.cend() || it2 != g2.cend()) {
      if (std::get<0>(*it1) != std::get<0>(*it2) || std::get<1>(*it1) != std::get<1>(*it2) ||
          std::get<2>(*it1) != std::get<2>(*it2)) {
        return false;
      }
      ++it1;
      ++it2;
    }
    return true;
  }

  friend bool operator!=(const Graph<N, E>& g1, const Graph<N, E>& g2) { return !(g1 == g2); }

  friend std::ostream& operator<<(std::ostream& os, const Graph<N, E>& g) {
    auto nodeList = g.GetNodes();
    size_t nodeCount = 0;

    const_iterator it = g.cbegin();
    if (it == g.cend())
      return os;

    auto currNode = nodeList[nodeCount];
    os << currNode << " (\n";
    while (it != g.cend()) {
      if (std::get<0>(*it) != currNode) {
        os << ")\n";
        ++nodeCount;
        while (nodeList[nodeCount] != std::get<0>(*it)) {
          // Check for nodes with no edges
          os << nodeList[nodeCount] << " (\n)\n";
          ++nodeCount;
        }
        currNode = nodeList[nodeCount];
        os << currNode << " (\n";
      }
      os << "  " << std::get<1>(*it) << " | " << std::get<2>(*it) << "\n";
      ++it;
    }
    os << ")\n";
    ++nodeCount;
    while (nodeCount != nodeList.size()) {
      // Check for nodes with no edges past end of edge iterator
      os << nodeList[nodeCount] << " (\n)\n";
      ++nodeCount;
    }

    return os;
  }

 private:
  std::set<std::shared_ptr<Node>, NodeComparator> nodes_;
  std::set<std::shared_ptr<Edge>, EdgeComparator> edges_;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
