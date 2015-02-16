#pragma once

#include <memory>

// Definitions for cleaner code (see Stephanov)
#define Function typename
#define Type typename

template <Type A>
struct List {
  explicit List(A data) : data{std::move(data)}, tail{nullptr} {}
  List(A data, std::shared_ptr<const List<A>> tail)
      : data{std::move(data)}, tail{std::move(tail)} {}
  const A data;
  const std::shared_ptr<const List<A>> tail;
};
template <Type A>
using ListPtr = std::shared_ptr<const List<A>>;

// cons :: a -> [a] -> [a]
template <Type A, typename... Args>
auto cons(A data, Args&&... args) -> ListPtr<A> {
  return std::make_shared<const List<A>>(std::move(data),
                                         std::forward<Args>(args)...);
}

// append :: [a] -> a -> [a]
template <Type A>
auto append(ListPtr<A> head, A&& data) -> ListPtr<A> {
  if (!head) {
    return cons<A>(std::forward<A>(data));
  }
  return cons<A>(head->data, append<A>(head->tail, std::forward<A>(data)));
}

// join :: [a] -> [a] -> [a]
template <Type A>
auto join(ListPtr<A> left, ListPtr<A> right) -> ListPtr<A> {
  if (!left) {
    return right;
  }
  return cons<A>(left->data, join<A>(left->tail, right));
}


// usually not a Haskell function but useful for printing etc.
// apply :: (a -> ()) -> [a] -> ()
template <Function FN, Type A>
auto apply(const FN& f, ListPtr<A> head) -> void {
  if (!head) {
    return;
  }
  f(head->data);
  apply<FN, A>(f, head->tail);
}

// fmap :: (a -> b) -> [a] -> [b]
template <Function FN, Type A, Type B = typename std::result_of<FN(A)>::type>
auto fmap(const FN& f, ListPtr<A> head) -> ListPtr<B> {
  if (!head) {
    return nullptr;
  }
  return cons<B>(f(head->data), fmap<FN, A, B>(f, head->tail));
}

// foldl :: (a -> b -> a) -> a -> [b] -> a
template <Function FN, Type A, Type B>
auto foldl(const FN& f, A&& acc, ListPtr<B> head) -> A {
  if (!head) {
    return std::forward<A>(acc);
  }
  return foldl<FN, A, B>(f, f(std::forward<A>(acc), head->data), head->tail);
}

// size :: [a] -> std::size_t
template <Type A>
auto size(ListPtr<A> head) -> std::size_t {
  return foldl([](const std::size_t& acc, const A&) {
    return acc + 1;
  }, 0u, head);
}

// empty :: [a] -> bool
template <Type A>
auto empty(ListPtr<A> head) -> bool {
  return head == nullptr;
}
