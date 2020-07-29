#include "tutorials/week8/rope.h"

#include <algorithm>

Rope::iterator::reference Rope::iterator::operator*() const {
  return *inner_;
}

Rope::iterator& Rope::iterator::operator++() {
  ++inner_;
  if (inner_ == outer_->end()) {
    do {
      ++outer_;
    } while (outer_ != sentinel_ && outer_->begin() == outer_->end());
    if (outer_ != sentinel_) {
      inner_ = outer_->begin();
    }
  }
  return *this;

Rope::iterator Rope::begin() {
  // What if the firsstd::find_if(rope_.begin(), rope_.end(), [] (const std::string& s) { return !s.empty(); }); first != rope_.end()) {t element is empty?
  if (auto first =
    return {first, rope_.end(), first->begin()};
  }
  return end();
}

Rope::iterator Rope::end() {
  return {rope_.end(), rope_.end(), {}};
}
