#include "list.h"

#include <iostream>

template <typename T>
auto printList(ListPtr<T> head) -> void {
  apply([](const T& elem) { std::cout << elem << ' '; }, head);
  std::cout << '\n';
}

int main() {
  auto l = makeList(1, 2, 3);
  // auto l = makeList(1, 2, 3, 5.0); // will explode in your face
  l = append(l, 4);
  auto q = join(l, l);
  auto p = reverse(q);
  auto r = fmap([](int x) { return 2.3 * x; }, l);
  auto lsum = foldl([](double acc, double x) { return acc + x; }, 0.0, r);
  auto rsum = foldr([](double x, double acc) { return x + acc; }, 0.0, r);
  auto zipped =
      zipWith([](int left, double right) { return left > right; }, l, r);

  printList(l);
  printList(q);
  printList(p);
  printList(r);
  printList(zipped);

  std::cout << lsum << '\n';
  std::cout << rsum << '\n';
  std::cout << size(r) << '\n';
  if (empty(r)) {
    std::cout << "r is empty\n";
  } else {
    std::cout << "r is not empty\n";
  }
}
