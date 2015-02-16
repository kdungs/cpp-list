#include "list.h"

#include <iostream>

template <typename T>
auto printList(ListPtr<T> head) -> void {
  apply([](const T& elem) { std::cout << elem << ' '; }, head);
  std::cout << '\n';
}

int main() {
  auto l = cons(4, cons(5, cons(6)));
  l = append(l, 7);
  auto q = join(l, l);
  auto r = fmap([](const int& x) { return 2.3 * x; }, l);
  auto sum =
      foldl([](const double& acc, const double& x) { return acc + x; }, 0.0, r);

  printList(l);
  printList(q);
  printList(r);

  std::cout << sum << '\n';
  std::cout << size(r) << '\n';
  if (empty(r)) {
    std::cout << "r is empty\n";
  } else {
    std::cout << "r is not empty\n";
  }
}
