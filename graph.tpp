#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

/*
 * 2.1 Constructors
 */
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator start,
                         typename std::vector<N>::const_iterator end) {
  for (auto it = start; it != end; ++it) {
    InsertNode(*it);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator start,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end) {
  for (auto it = start; it != end; ++it) {
    this->InsertNode(std::get<0>(*it));
    this->InsertNode(std::get<1>(*it));
    this->InsertEdge(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it));
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> list) {
  for (auto it = list.begin(); it != list.end(); ++it) {
    InsertNode(*it);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const Graph<N, E>& g) {
  for (auto n = g.nodes_.begin(); n != g.nodes_.end(); ++n) {
    InsertNode((*n)->value_);
  }
  for (auto e = g.edges_.begin(); e != g.edges_.end(); ++e) {
    InsertEdge((*e)->src_.lock()->value_, (*e)->dst_.lock()->value_, (*e)->weight_);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(Graph<N, E>&& g) {
  nodes_ = std::move(g.nodes_);
  edges_ = std::move(g.edges_);
}

/*
 * 2.2 Operations
 */
// Copy Assignment
template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(const Graph<N, E>& g) {
  if (this != &g) {
    Clear();
    for (auto n = g.nodes_.begin(); n != g.nodes_.end(); ++n) {
      InsertNode((*n)->value_);
    }
    for (auto e = g.edges_.begin(); e != g.edges_.end(); ++e) {
      InsertEdge((*e)->src_.lock()->value_, (*e)->dst_.lock()->value_, (*e)->weight_);
    }
  }
  return *this;
}
// Move Assignment
template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(Graph<N, E>&& g) {
  if (this != &g) {
    nodes_ = std::move(g->nodes_);
    edges_ = std::move(g->edges_);
  }
  return *this;
}

/*
 * 2.3 Methods
 */
template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) noexcept {
  //  if not in graph
  //  std_make_shared<N>(n);
  Node n{val};
  auto res = nodes_.insert(std::make_shared<Node>(n));
  return res.second;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw(std::runtime_error(
        "Cannot call Graph::InsertEdge when either src or dst node does not exist"));
  }

  auto s = nodes_.find(std::make_shared<Node>(src));
  auto d = nodes_.find(std::make_shared<Node>(dst));

  Edge e{*s, *d, w};
  auto res = edges_.insert(std::make_shared<Edge>(e));
  return res.second;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& val) noexcept {
  if (!IsNode(val)) {
    return false;
  }

  // Erase corresponding edges
  for (auto e = edges_.begin(); e != edges_.end();) {
    if ((*e)->src_.lock()->value_ == val || (*e)->dst_.lock()->value_ == val) {
      e = edges_.erase(e);
    } else {
      ++e;
    }
  }

  // Erase node
  for (auto n = nodes_.begin(); n != nodes_.end(); ++n) {
    if ((*n)->value_ == val) {
      nodes_.erase(n);
      break;
    }
  }

  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw(std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist"));
  }

  if (IsNode(newData))
    return false;
  InsertNode(newData);
  // Replace edges containing oldData
  for (auto e = edges_.begin(); e != edges_.end(); ++e) {
    if ((*e)->src_.lock()->value_ == oldData && (*e)->dst_.lock()->value_ == oldData) {
      InsertEdge(newData, newData, (*e)->weight_);
    } else if ((*e)->src_.lock()->value_ == oldData && (*e)->dst_.lock()->value_ != oldData) {
      InsertEdge(newData, (*e)->dst_.lock()->value_, (*e)->weight_);
    } else if ((*e)->src_.lock()->value_ != oldData && (*e)->dst_.lock()->value_ == oldData) {
      InsertEdge((*e)->src_.lock()->value_, newData, (*e)->weight_);
    } else {
      continue;  // do nothing to this edge
    }
  }
  DeleteNode(oldData);
  return true;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw(std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph"));
  }

  if (!IsNode(newData)) {
    std::cout << "just do replace" << '\n';
    Replace(oldData, newData);
    return;
  }

  Node ne{newData};

  auto n = nodes_.find(std::make_shared<Node>(ne));

  // Replace edges containing oldData
  for (auto e = edges_.begin(); e != edges_.end(); ++e) {
    if ((*e)->src_.lock()->value_ != oldData && (*e)->dst_.lock()->value_ != oldData)
      continue;

    bool src = false;
    bool dst = false;
    if ((*e)->src_.lock()->value_ == oldData)
      src = true;
    if ((*e)->dst_.lock()->value_ == oldData)
      dst = true;

    if (src && dst) {
      Edge ed{*n, *n, (*e)->weight_};
      if (edges_.find(std::make_shared<Edge>(ed)) == edges_.end()) {
        InsertEdge(newData, newData, (*e)->weight_);
      }
    } else if (src && !dst) {
      Edge ed{*n, (*e)->dst_.lock(), (*e)->weight_};
      if (edges_.find(std::make_shared<Edge>(ed)) == edges_.end()) {
        InsertEdge(newData, (*e)->dst_.lock()->value_, (*e)->weight_);
      }
    } else if (!src && dst) {
      Edge ed{(*e)->src_.lock(), *n, (*e)->weight_};
      if (edges_.find(std::make_shared<Edge>(ed)) == edges_.end()) {
        InsertEdge(newData, (*e)->dst_.lock()->value_, (*e)->weight_);
      }
    } else {
      continue;
    }
  }
  DeleteNode(oldData);
}

template <typename N, typename E>
void gdwg::Graph<N, E>::Clear() noexcept {
  nodes_.clear();
  edges_.clear();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& val) const noexcept {
  Node n{val};
  return (nodes_.find(std::make_shared<Node>(n)) != nodes_.end());
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw(std::runtime_error(
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph"));
  }
  for (const auto& e : edges_) {
    if (e->src_.lock()->value_ == src && e->dst_.lock()->value_ == dst) {
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() const noexcept {
  std::vector<N> res;
  for (const auto& n : nodes_) {
    res.push_back(n->value_);
  }
  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) const {
  if (!IsNode(src)) {
    throw(std::runtime_error("Cannot call Graph::GetConnected if src doesn't exist in the graph"));
  }
  std::vector<N> res;
  for (const auto& e : edges_) {
    if (src == e->src_.lock()->value_) {
      res.push_back(e->dst_.lock()->value_);
    }
  }
  return res;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw(std::runtime_error(
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph"));
  }
  std::vector<E> res;

  for (auto e = edges_.begin(); e != edges_.end(); ++e) {
    if ((*e)->src_.lock()->value_ == src && (*e)->dst_.lock()->value_ == dst) {
      res.push_back((*e)->weight_);
    }
  }
  return res;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) noexcept {
  for (auto e = edges_.begin(); e != edges_.end(); ++e) {
    if ((*e)->src_.lock()->value_ == src && (*e)->dst_.lock()->value_ == dst &&
        (*e)->weight_ == w) {
      edges_.erase(e);
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& n1, const N& n2, const E& e) {
  // Returns an iterator to the found edge in the graph.
  // If the edge is not found the equivalent value of gdwg::Graph<N, E>::cend() is returned.
  auto edge = edges_.begin();
  for (; edge != edges_.end(); ++edge) {
    if ((edge->get()->weight_ == e) && (edge->get()->src_.lock()->value_ == n1) &&
        (edge->get()->dst_.lock()->value_ == n2)) {
      auto node1 = std::find(nodes_.begin(), nodes_.end(), edge->get()->src_.lock());
      auto node2 = std::find(nodes_.begin(), nodes_.end(), edge->get()->dst_.lock());
      return {node1, node2, edge, edges_.end()};
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::erase(gdwg::Graph<N, E>::const_iterator it) {
  for (auto e = edges_.begin(); e != edges_.end(); ++e) {
    if ((*e)->get()->src_.lock()->value_ == std::get<0>(*it) &&
        (*e)->get()->dst_.lock()->value_ == std::get<1>(*it) &&
        (*e)->get()->weight_ == std::get<2>(*it)) {
      auto next = e;
      edges_.erase(e);
      ++next;
      if (next == edges_.end()) {
        return end();
      }
      auto node1 = std::find(nodes_.begin(), nodes_.end(), next->get()->src_.lock());
      auto node2 = std::find(nodes_.begin(), nodes_.end(), next->get()->dst_.lock());
      return {node1, node2, next, edges_.end()};
    }
  }
  return end();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {
  for (auto n : nodes_) {
    for (auto e : edges_) {
      if (e->src_.lock() == n) {
        auto edge = std::find(edges_.begin(), edges_.end(), e);
        auto node1 = std::find(nodes_.begin(), nodes_.end(), e->src_.lock());
        auto node2 = std::find(nodes_.begin(), nodes_.end(), e->dst_.lock());
        return {node1, node2, edge, edges_.end()};
      }
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {
  return gdwg::Graph<N, E>::const_iterator({}, {}, edges_.end(), edges_.end());
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::begin() {
  const auto& node1 = nodes_.begin();
  auto edge = edges_.begin();
  for (; edge != edges_.end(); ++edge) {
    if (edge->get()->src_.lock() == *node1) {
      auto node2 = std::find(nodes_.begin(), nodes_.end(), edge->get()->dst_.lock());
      return {node1, node2, edge, edges_.end()};
    }
  }
  return end();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::end() {
  return gdwg::Graph<N, E>::const_iterator({}, {}, edges_.end(), edges_.end());
}
