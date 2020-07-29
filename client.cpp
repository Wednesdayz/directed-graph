#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "assignments/dg/graph.h"

// Note: At the moment, there is no client.sampleout. Please do your own testing

int main() {
  int i = 1;
  int j = 2;

  int& k = i;
  k = j; // This does not make k reference j instead of i. It just changes the value.
  std::cout << "i = " << i << ", j = " << j << ", k = " << k << '\n';

//  gdwg::Graph<std::string, int> g;
//  g.InsertNode("hello");
//  g.InsertNode("how");
//  g.InsertNode("are");
//  g.InsertNode("you?");
//
//  g.InsertEdge("hello", "how", 5);
//  g.InsertEdge("hello", "are", 8);
//  g.InsertEdge("hello", "are", 2);
//
//  g.InsertEdge("how", "you?", 1);
//  g.InsertEdge("how", "hello", 4);
//
//  g.InsertEdge("are", "you?", 3);
//
//  std::cout << g << '\n';
//
//  gdwg::Graph<std::string, int> g2{g};
//
//  std::cout << g2 << "\n";
//
//  assert(g == g2);
//
//  g.Replace("hello", "hi");
//  assert(g.IsNode("hi"));
//  assert(!g.IsNode("hello"));
//  std::cout << "REPLACED" << '\n';
//  std::cout << g << '\n';
//
//  g.DeleteNode("hello");
//  std::cout << "DELETED" << '\n';
//  std::cout << g << '\n';
//
//  g.erase("are", "you?", 3);
//  std::cout << "ERASED" << '\n';
//  assert(g.GetWeights("how", "you?")[0] == 1);
//  assert(g.GetConnected("you?").size() == 0);
//  std::cout << "Here" << '\n';
//  std::cout << g << '\n';
//
//  assert(g != g2);
//
//  // This is a structured binding.
//  // https://en.cppreference.com/w/cpp/language/structured_binding
//  // It allows you to unpack your tuple.
//  for (const auto& [from, to, weight] : g) {
//    std::cout << from << " -> " << to << " (weight " << weight << ")\n";
//  }
//
//  gdwg::Graph<std::string, int> g3;
//  {
//    std::string s1{"Hello"};
//    g3.InsertNode(s1);
//  }
//  assert(g3.IsNode("Hello") == true);
//  assert(g3.GetNodes().size() == 1);
//  std::cout << "before delete" << '\n';
//  g3.DeleteNode("Hello");
//  std::cout << "after delete" << '\n';
//  assert(g3.IsNode("Hello") == false);
//  assert(g3.GetNodes().size() == 0);
//
//  // =======//
//  std::string s1{"Hello"};
//  std::string s2{"how"};
//  std::string s3{"are"};
//  auto e1 = std::make_tuple(s1, s2, 5.4);  // Hello-how
//  auto e2 = std::make_tuple(s2, s3, 7.6);  // how-are
//  auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
//  gdwg::Graph<std::string, double> g4{e.begin(), e.end()};
//
//  std::cout << "before replace" << '\n';
//  std::cout << g4 << '\n';
//
//  assert(g4.Replace("Hello", "hi"));
//  assert(g4.GetNodes().size() == 3);
//  assert(!g4.IsNode("Hello"));
//  assert(g4.IsNode("hi"));
//  assert(g4.IsConnected("hi", "how"));  // hi-how
//  assert(g4.IsConnected("how", "are"));
//
//  std::cout << "after replace" << '\n';
//  std::cout << g4 << '\n';
//
//  std::cout << "trying merge replace" << '\n';
//  g4.MergeReplace("how", "hi");
//  std::cout << "maybe succeeded?" << '\n';
//  std::cout << g4 << '\n';
//  assert(!g4.IsNode("how"));
//  assert(g4.IsNode("hi"));
//  std::cout << g4.GetNodes().size() << '\n';
//  assert(g4.GetWeights("hi", "are").size() == 1);
//  std::cout << "success!" << '\n';
}
