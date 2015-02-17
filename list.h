#pragma once

#include <cassert>
#include <memory>
#include <utility>

// Definitions for cleaner code (see Stephanov)
#define Type typename
#define Function typename
#define Predicate typename

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
// cons(cons(...)) all the time. Equivalent to [1, 2, 3] vs 1 : 2 : 3 : () in
// Haskell.
template <Type A>
auto makeList(A&& data) -> ListPtr<A> {
  return cons<A>(std::forward<A>(data));
}
template <Type A, typename... Args>
auto makeList(A&& data, Args&&... args) -> ListPtr<A> {
  return cons<A>(std::forward<A>(data), makeList(std::forward<Args>(args)...));
}

// ---------------
// List operations
// ---------------

// map :: (a -> b) -> [a] -> [b]
template <Function FN, Type A, Type B = typename std::result_of<FN(A)>::type>
auto map(const FN& f, const ListPtr<A>& head) -> ListPtr<B> {
  if (!head) {
    return nullptr;
  }
  return cons<B>(f(head->data), map<FN, A, B>(f, head->tail));
}

// append :: [a] -> a -> [a]
template <Type A>
auto append(const ListPtr<A>& head, A&& data) -> ListPtr<A> {
  if (!head) {
    return cons<A>(std::forward<A>(data));
  }
  return cons<A>(head->data, append<A>(head->tail, std::forward<A>(data)));
}

// (++) :: [a] -> [a] -> [a]
template <Type A>
auto join(const ListPtr<A>& left, const ListPtr<A>& right) -> ListPtr<A> {
  if (!left) {
    return right;
  }
  return cons<A>(left->data, join<A>(left->tail, right));
}

// filter :: (a -> Bool) -> [a] -> [a]
template <Predicate PR, Type A>
auto filter(const PR& p, const ListPtr<A>& head) -> ListPtr<A> {
  if (!head) {
    return nullptr;
  }
  auto tail = filter<PR, A>(p, head->tail);
  if (p(head->data)) {
    return cons<A>(head->data, tail);
  }
  return tail;
}

// head :: [a] -> a
template <Type A>
auto head(const ListPtr<A>& head) -> A {
  assert(head && "List can't be empty.");
  return head->data;
}

// last :: [a] -> a
template <Type A>
auto last(const ListPtr<A>& head) -> A {
  assert(head && "List can't be empty.");
  if (!head->tail) {
    return head->data;
  }
  return last(head->tail);
}

// tail :: [a] -> [a]
template <Type A>
auto tail(const ListPtr<A>& head) -> ListPtr<A> {
  assert(head && "List can't be empty.");
  return head->tail;
}

// init :: [a] -> [a]
template <Type A>
auto init(const ListPtr<A>& head) -> ListPtr<A> {
  assert(head && "List can't be empty.");
  if (!head->tail) {
    return nullptr;
  }
  return cons<A>(head->data, init(head->tail));
}

// null :: [a] -> Bool
template <Type A>
auto null(const ListPtr<A>& head) -> bool {
  return head == nullptr;
}

// length :: [a] -> Int
template <Type A>
auto length(const ListPtr<A>& head) -> std::size_t {
  return foldl([](const std::size_t& acc, const A&) { return acc + 1; }, 0u,
               head);
}

// (!!) :: [a] -> Int -> a
template <Type A>
auto at(const ListPtr<A>& head, std::size_t index) -> A {
  assert(head &&
         "List can't be empty and index must be smaller than size of list.");
  if (index == 0) {
    return head->data;
  }
  return at(head, index - 1);
}

// reverse uses fold and is thus defined later on

// ----------------------
// Reducing lists (folds)
// ----------------------

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

// -------------
// Special folds
// -------------

// concat :: [[a]] -> [a]
template <Type A>
auto concat(const ListPtr<ListPtr<A>>& head) -> ListPtr<A> {
  auto _concat = [](const ListPtr<A>& acc, const ListPtr<A>& list) {
    return join<A>(acc, list);
  };
  return foldl<decltype(_concat), ListPtr<A>, ListPtr<A>>(_concat, nullptr,
                                                          head);
}

// concatMap :: (a -> [b]) -> [a] -> [b]
template <Function FN, Type A,
          Type ListB = typename std::result_of<FN(A)>::type>
auto concatMap(const FN& f, const ListPtr<A>& head) -> ListB {
  return concat(map(f, head));
}

// ---------------------------
// Zipping and unzipping lists
// ---------------------------

// zip :: [a] -> [b] -> [(a, b)]
template <Type A, Type B, typename TUP = std::tuple<A, B>>
auto zip(const ListPtr<A>& left, const ListPtr<B>& right) -> ListPtr<TUP> {
  if (!left || !right) {
    return nullptr;
  }
  return cons<TUP>(std::make_tuple(left->data, right->data),
                   zip<A, B, TUP>(left->tail, right->tail));
}

// zip3 :: [a] -> [b] -> [c] -> [(a, b, c)]
template <Type A, Type B, Type C, typename TUP = std::tuple<A, B, C>>
auto zip3(const ListPtr<A>& left, const ListPtr<B>& middle,
          const ListPtr<C>& right) -> ListPtr<TUP> {
  if (!left || !middle || !right) {
    return nullptr;
  }
  return cons<TUP>(std::make_tuple(left->data, middle->data, right->data),
                   zip3<A, B, C, TUP>(left->tail, middle->tail, right->tail));
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

// zipWith3 :: (a -> b -> c -> d) -> [a] -> [b] -> [c] -> [d]
template <Function FN, Type A, Type B, Type C,
          Type D = typename std::result_of<FN(A, B, C)>::type>
auto zipWith3(const FN& f, const ListPtr<A>& left, const ListPtr<B>& middle,
              const ListPtr<C>& right) -> ListPtr<D> {
  if (!left || !middle || !right) {
    return nullptr;
  }
  return cons<D>(
      f(left->data, middle->data, right->data),
      zipWith3<FN, A, B, C, D>(f, left->tail, middle->tail, right->tail));
}

// unzip :: [(a, b)] -> ([a], [b])
template <typename TUP, Type A = typename std::tuple_element<0, TUP>::type,
          Type B = typename std::tuple_element<1, TUP>::type,
          typename LTUP = std::tuple<ListPtr<A>, ListPtr<B>>>
auto unzip(const ListPtr<TUP>& head) -> LTUP {
  if (!head) {
    return LTUP{nullptr, nullptr};
  }
  auto rest = unzip<TUP, A, B, LTUP>(head->tail);
  return LTUP{cons<A>(std::get<0>(head->data), std::get<0>(rest)),
              cons<B>(std::get<1>(head->data), std::get<1>(rest))};
}

// unzip3 :: [(a, b, c)] -> ([a], [b], [c])
template <typename TUP, Type A = typename std::tuple_element<0, TUP>::type,
          Type B = typename std::tuple_element<1, TUP>::type,
          Type C = typename std::tuple_element<2, TUP>::type,
          typename LTUP = std::tuple<ListPtr<A>, ListPtr<B>, ListPtr<C>>>
auto unzip3(const ListPtr<TUP>& head) -> LTUP {
  if (!head) {
    return LTUP{nullptr, nullptr, nullptr};
  }
  auto rest = unzip3<TUP, A, B, C, LTUP>(head->tail);
  return LTUP{cons<A>(std::get<0>(head->data), std::get<0>(rest)),
              cons<B>(std::get<1>(head->data), std::get<1>(rest)),
              cons<C>(std::get<2>(head->data), std::get<2>(rest))};
}

// -----
// Other
// -----

// reverse :: [a] -> [a]
template <Type A>
auto reverse(const ListPtr<A>& head) -> ListPtr<A> {
  if (!head->tail) {
    return head;
  }
  auto f = [](ListPtr<A> acc, const A& data) { return cons<A>(data, acc); };
  return foldl<decltype(f), ListPtr<A>, A>(f, nullptr, head);
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
