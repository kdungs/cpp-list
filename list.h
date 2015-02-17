#pragma once

#include <cassert>
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

// Variadic helper function to create lists without having to write
// cons(cons(...)) all the time.
template <Type A>
auto makeList(A&& data) -> ListPtr<A> {
  return cons<A>(std::forward<A>(data));
}
template <Type A, typename... Args>
auto makeList(A&& data, Args&&... args) -> ListPtr<A> {
  return cons<A>(std::forward<A>(data), makeList(std::forward<Args>(args)...));
}

// append :: [a] -> a -> [a]
template <Type A>
auto append(const ListPtr<A>& head, A&& data) -> ListPtr<A> {
  if (!head) {
    return cons<A>(std::forward<A>(data));
  }
  return cons<A>(head->data, append<A>(head->tail, std::forward<A>(data)));
}

// join :: [a] -> [a] -> [a]
template <Type A>
auto join(const ListPtr<A>& left, const ListPtr<A>& right) -> ListPtr<A> {
  if (!left) {
    return right;
  }
  return cons<A>(left->data, join<A>(left->tail, right));
}

// usually not a Haskell function but useful for printing etc.
// apply :: (a -> ()) -> [a] -> ()
template <Function FN, Type A>
auto apply(const FN& f, const ListPtr<A>& head) -> void {
  if (!head) {
    return;
  }
  f(head->data);
  apply<FN, A>(f, head->tail);
}

// fmap :: (a -> b) -> [a] -> [b]
template <Function FN, Type A, Type B = typename std::result_of<FN(A)>::type>
auto fmap(const FN& f, const ListPtr<A>& head) -> ListPtr<B> {
  if (!head) {
    return nullptr;
  }
  return cons<B>(f(head->data), fmap<FN, A, B>(f, head->tail));
}

// foldl :: (a -> b -> a) -> a -> [b] -> a
template <Function FN, Type A, Type B>
auto foldl(const FN& f, A&& acc, const ListPtr<B>& head) -> A {
  if (!head) {
    return std::forward<A>(acc);
  }
  return foldl<FN, A, B>(f, f(std::forward<A>(acc), head->data), head->tail);
}

// foldl1 :: (a -> a -> a) -> [a] -> a
template <Function FN, Type A>
auto foldl1(const FN& f, const ListPtr<A>& head) -> A {
  assert(head && "List can't be empty.");
  return foldl<FN, const A&, A>(f, head->data, head->tail);
}

// foldr :: (a -> b -> b) -> b -> [a] -> b
template <Function FN, Type A, Type B>
auto foldr(const FN& f, B&& acc, const ListPtr<A>& head) -> B {
  if (!head) {
    return std::forward<B>(acc);
  }
  return f(head->data, foldr<FN, A, B>(f, std::forward<B>(acc), head->tail));
}

// foldr1 :: (a -> a -> a) -> [a] -> a
template <Function FN, Type A>
auto foldr1(const FN& f, const ListPtr<A>& head) -> A {
  assert(head && "List can't be empty.");
  if (!head->tail) {
    return head->data;
  }
  return f(head->data, foldr1<FN, A>(f, head->tail));
}

// size :: [a] -> std::size_t
template <Type A>
auto size(const ListPtr<A>& head) -> std::size_t {
  return foldl([](const std::size_t& acc, const A&) { return acc + 1; }, 0u,
               head);
}

// empty :: [a] -> bool
template <Type A>
auto empty(const ListPtr<A>& head) -> bool {
  return head == nullptr;
}

// reverse :: [a] -> [a]
template <Type A>
auto reverse(const ListPtr<A>& head) -> ListPtr<A> {
  if (!head->tail) {
    return head;
  }
  auto f = [](ListPtr<A> acc, const A& data) { return cons<A>(data, acc); };
  return foldl<decltype(f), ListPtr<A>, A>(f, nullptr, head);
}

// zipWith :: (a -> b -> c) -> [a] -> [b] -> [c]
template <Function FN, Type A, Type B,
          Type C = typename std::result_of<FN(A, B)>::type>
auto zipWith(const FN& f, const ListPtr<A>& left, const ListPtr<B>& right)
    -> ListPtr<C> {
  if (!left || !right) {
    return nullptr;
  }
  return cons<C>(f(left->data, right->data),
                 zipWith<FN, A, B, C>(f, left->tail, right->tail));
}
