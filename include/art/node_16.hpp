/**
 * @file node_16 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_16_HPP
#define ART_NODE_16_HPP

#include "node.hpp"
#include <array>
#include <stdexcept>
#include <utility>

namespace art {

using std::array;

template <class T> class node_4;
template <class T> class node_48;

template <class T> class node_16 : public node<T> {
public:
  node<T> **find_child(uint8_t partial_key) override;
  void set_child(uint8_t partial_key, node<T> *child) override;
  node<T> *del_child(uint8_t partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  uint8_t next_partial_key(uint8_t partial_key) const override;

  uint8_t prev_partial_key(uint8_t partial_key) const override;

  int n_children() const override;

private:
  uint8_t n_children_ = 0;
  array<uint8_t, 16> keys_;
  array<node<T> *, 16> children_;
};

template <class T> node<T> **node_16<T>::find_child(uint8_t partial_key) {
  for (int i = 0; i < n_children_; ++i) {
    if (keys_[i] == partial_key) {
      return &children_[i];
    }
  }
  return nullptr;
}

template <class T>
void node_16<T>::set_child(uint8_t partial_key, node<T> *child) {
  /* determine index for child */
  int child_i;
  for (int i = this->n_children_ - 1;; --i) {
    if (i >= 0 && partial_key < this->keys_[i]) {
      /* move existing sibling to the right */
      this->keys_[i + 1] = this->keys_[i];
      this->children_[i + 1] = this->children_[i];
    } else {
      child_i = i + 1;
      break;
    }
  }

  this->keys_[child_i] = partial_key;
  this->children_[child_i] = child;
  ++n_children_;
}

template <class T> node<T> *node_16<T>::del_child(uint8_t partial_key) {
  node<T> *child_to_delete = nullptr;
  for (int i = 0; i < n_children_; ++i) {
    if (child_to_delete == nullptr && partial_key == keys_[i]) {
      child_to_delete = children_[i];
    }
    if (child_to_delete != nullptr) {
      /* move existing sibling to the left */
      keys_[i] = i < n_children_ - 1 ? keys_[i + 1] : 0;
      children_[i] = i < n_children_ - 1 ? children_[i + 1] : nullptr;
    }
  }
  if (child_to_delete != nullptr) {
    --n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_16<T>::grow() {
  auto new_node = new node_48<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  for (int i = 0; i < n_children_; ++i) {
    new_node->set_child(keys_[i], children_[i]);
  }
  delete this;
  return new_node;
}

template <class T> node<T> *node_16<T>::shrink() {
  auto new_node = new node_4<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  for (int i = 0; i < n_children_; ++i) {
    new_node->set_child(keys_[i], children_[i]);
  }
  delete this;
  return new_node;
}

template <class T> bool node_16<T>::is_full() const {
  return n_children_ == 16;
}

template <class T> bool node_16<T>::is_underfull() const {
  return n_children_ == 4;
}

template <class T>
uint8_t node_16<T>::next_partial_key(uint8_t partial_key) const {
  for (int i = 0; i < n_children_; ++i) {
    if (keys_[i] >= partial_key) {
      return keys_[i];
    }
  }
  throw std::out_of_range("provided partial key does not have a successor");
}

template <class T>
uint8_t node_16<T>::prev_partial_key(uint8_t partial_key) const {
  for (int i = n_children_ - 1; i >= 0; --i) {
    if (keys_[i] <= partial_key) {
      return keys_[i];
    }
  }
  throw std::out_of_range("provided partial key does not have a predecessor");
}

template <class T> int node_16<T>::n_children() const { return n_children_; }

} // namespace art

#endif
