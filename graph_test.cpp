/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/
#include <string>
#include <utility>

#include "assignments/dg/graph.h"
#include "catch.h"

SCENARIO("Default Construction of identical graphs") {
  WHEN("Using the copy constructor to make two same graphs") {
    gdwg::Graph<std::string, int> g;
    REQUIRE(g.GetNodes().size() == 0);

    g.InsertNode("hello");
    g.InsertNode("how");
    g.InsertNode("are");
    g.InsertNode("you?");
    REQUIRE(g.GetNodes().size() == 4);

    gdwg::Graph<std::string, int> g2{g};

    THEN("They should be equal") { REQUIRE(g == g2); }
    THEN("The should have the same nodes") { REQUIRE(g.GetNodes().size() == g2.GetNodes().size()); }
  }
}

SCENARIO("Default Construction of different graphs") {
  WHEN("Two different graphs are created containing some same nodes") {
    gdwg::Graph<std::string, int> g;
    {
      std::string s1{"Hello"};
      g.InsertNode(s1);
    }
    REQUIRE(g.GetNodes().size() == 1);
    REQUIRE(g.IsNode("Hello"));

    gdwg::Graph<std::string, int> g2{g};
    g2.InsertNode("hi");
    REQUIRE(g2.GetNodes().size() == 2);

    THEN("They should not be equal") { REQUIRE(g != g2); }
    THEN("They should have their own copies of the same node") {
      REQUIRE(g.DeleteNode("Hello"));
      REQUIRE(g.IsNode("Hello") == false);
      REQUIRE(g2.IsNode("Hello") == true);
      REQUIRE(g.GetNodes().size() == 0);
      REQUIRE(g2.GetNodes().size() == 2);
    }
  }
}

SCENARIO("Vector Iterator Construction of a graphs") {
  WHEN("A graph is created from auto vector") {
    std::vector<std::string> v{"hello", "how", "are", "you?"};
    gdwg::Graph<std::string, double> g{v.begin(), v.end()};

    REQUIRE(g.GetNodes().size() == 4);
    REQUIRE(g.IsNode("hello"));
    REQUIRE(g.IsNode("how"));
    REQUIRE(g.IsNode("are"));
    REQUIRE(g.IsNode("you?"));

    g.InsertEdge("hello", "how", 5);
    g.InsertEdge("hello", "are", 8);
    g.InsertEdge("hello", "are", 2);
    g.InsertEdge("how", "you?", 1);
    g.InsertEdge("how", "hello", 4);
    g.InsertEdge("are", "you?", 3);

    THEN("Check the edges are created") {
      REQUIRE(g.IsConnected("hello", "how"));
      REQUIRE(g.IsConnected("hello", "are"));
      REQUIRE(g.IsConnected("how", "you?"));
      REQUIRE(!g.IsConnected("you?", "how"));
      REQUIRE(g.IsConnected("how", "hello"));
      REQUIRE(g.IsConnected("are", "you?"));

      REQUIRE(g.GetConnected("hello").size() == 3);
      REQUIRE(g.GetConnected("how").size() == 2);
      REQUIRE(g.GetConnected("are").size() == 1);
      REQUIRE(g.GetConnected("you?").size() == 0);
    }
    THEN("Check the edges' weights") {
      std::vector<double> weights = g.GetWeights("hello", "are");
      REQUIRE(weights.size() == 2);
      REQUIRE(weights[0] == 2);
      REQUIRE(weights[1] == 8);
    }
    THEN("Try copy assingment") {
      gdwg::Graph<std::string, double> g2 = g;
      REQUIRE(g == g2);
    }
    THEN("Try move assignment") {
      gdwg::Graph<std::string, double> g3 = std::move(g);
      REQUIRE(g3 != g);
      REQUIRE(g.GetNodes().size() == 0);
    }
  }
}

SCENARIO("Iterators Over Tuples Constructor") {
  WHEN("A graph is created from tuples") {
    std::string s1{"Hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    auto e1 = std::make_tuple(s1, s2, 5.4);
    auto e2 = std::make_tuple(s2, s3, 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> g{e.begin(), e.end()};

    REQUIRE(g.GetNodes().size() == 3);

    THEN("Check the edges are created") {
      REQUIRE(g.IsConnected("Hello", "how"));
      REQUIRE(g.IsConnected("how", "are"));
      REQUIRE(!g.IsConnected("Hello", "are"));
    }
    THEN("Check the edges' weights") {
      std::vector<double> weight1 = g.GetWeights("Hello", "how");
      std::vector<double> weight2 = g.GetWeights("how", "are");
      REQUIRE(weight1.size() == 1);
      REQUIRE(weight1[0] == 5.4);
      REQUIRE(weight2.size() == 1);
      REQUIRE(weight2[0] == 7.6);
    }
    THEN("Try inserting duplicate edges") {
      REQUIRE(!g.InsertEdge(s1, s2, 5.4));
      REQUIRE(g.GetWeights(s1, s2).size() == 1);
      REQUIRE(g.InsertEdge(s2, s1, 7.6));
      REQUIRE(g.GetWeights(s2, s1).size() == 1);
    }
    THEN("Try replacing node then MergeReplacing") {
      REQUIRE(g.Replace("Hello", "hi"));
      REQUIRE(g.GetNodes().size() == 3);
      REQUIRE(!g.IsNode("Hello"));
      REQUIRE(g.IsNode("hi"));
      REQUIRE(g.IsConnected("hi", "how"));
      REQUIRE(g.IsConnected("how", "are"));

      g.MergeReplace("how", "hi");
      REQUIRE(g.GetNodes().size() == 2);
      REQUIRE(!g.IsNode("how"));
      REQUIRE(g.IsNode("hi"));
      REQUIRE(g.IsConnected("hi", "are"));
      REQUIRE(g.GetWeights("hi", "are").size() == 1);
      REQUIRE(g.GetWeights("hi", "are")[0] == 7.6);
    }
    THEN("Erase an edge") {
      REQUIRE(g.erase("how", "are", 7.6));
      REQUIRE(g.GetWeights("how", "are").size() == 0);
      REQUIRE(!g.IsConnected("how", "are"));
      REQUIRE(g.IsNode("how"));
      REQUIRE(g.IsNode("are"));
    }
    THEN("Clear the graph") {
      g.Clear();
      REQUIRE(g.GetNodes().size() == 0);
    }
  }
}
