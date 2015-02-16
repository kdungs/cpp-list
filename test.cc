#include "list.h"

#include <iostream>

int main() {
  auto l = cons(4, cons(5, cons(6)));
  l = append(l, 7);
  auto q = join(l, l);
  auto r = fmap([](const int& x) { return 2.3 * x; }, l);
  auto sum =
      foldl([](const double& acc, const double& x) { return acc + x; }, 0.0, r);

  apply([](const int& elem) { std::cout << elem << ' '; }, l);
  std::cout << '\n';
  apply([](const int& elem) { std::cout << elem << ' '; }, q);
  std::cout << '\n';
  apply([](const double& elem) { std::cout << elem << ' '; }, r);
  std::cout << '\n';

  std::cout << sum << '\n';
  std::cout << size(r) << '\n';
  if (empty(r)) {
    std::cout << "r is empty\n";
  } else {
    std::cout << "r is not empty\n";
  }
}
