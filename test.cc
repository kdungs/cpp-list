#include "list.h"

#include <iostream>

template <typename T>
auto printList(ListPtr<T> head) -> void {
  apply([](const T& elem) { std::cout << elem << ' '; }, head);
  std::cout << '\n';
}

int main() {
  auto l = cons(1, cons(2, cons(3)));
  l = append(l, 4);
  auto q = join(l, l);
  auto p = reverse(q);
  auto r = fmap([](const int& x) { return 2.3 * x; }, l);
  auto lsum = foldl([](const double& acc, const double& x) {
    std::cout << " +" << x << '\n';
    return acc + x;
  }, 0.0, r);
  auto rsum = foldr([](const double& x, const double& acc) {
    std::cout << " +" << x << '\n';
    return x + acc;
  }, 0.0, r);

  printList(l);
  printList(q);
  printList(p);
  printList(r);

  std::cout << lsum << '\n';
  std::cout << rsum << '\n';
  std::cout << size(r) << '\n';
  if (empty(r)) {
    std::cout << "r is empty\n";
  } else {
    std::cout << "r is not empty\n";
  }
}
