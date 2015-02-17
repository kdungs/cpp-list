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
  auto r = map([](int x) { return 2.3 * x; }, l);
  auto filtered = filter([](int x) { return x % 2 == 0; }, l);
  auto zipped =
      zipWith([](int left, double right) { return left > right; }, l, r);

  printList(l);
  printList(q);
  printList(p);
  printList(r);
  printList(filtered);
  printList(zipped);
  
  auto lsum = foldl([](double acc, double x) { return acc + x; }, 0.0, r);
  auto lsum1 = foldl1([](double acc, double x) { return acc + x; }, r);
  auto rsum = foldr([](double x, double acc) { return x + acc; }, 0.0, r);
  auto rsum1 = foldr1([](double x, double acc) { return x + acc; }, r);
  std::cout << "lsum = " << lsum << '\n';
  std::cout << "lsum1 = " << lsum1 << '\n';
  std::cout << "rsum = " << rsum << '\n';
  std::cout << "rsum1 = " << rsum1 << '\n';

  auto ldiv = foldl([](double acc, double x) { return acc / x;}, 1.0, r);
  auto rdiv = foldr([](double acc, double x) { return acc / x;}, 1.0, r);
  std::cout << "ldiv = " << ldiv << '\n';
  std::cout << "rdiv = " << rdiv << '\n';

  std::cout << "length(r) = " << length(r) << '\n';
  if (null(r)) {
    std::cout << "r is empty\n";
  } else {
    std::cout << "r is not empty\n";
  }

  std::cout << "head(r) = " << head(r) << '\n';
  std::cout << "last(r) = " << last(r) << '\n';
  std::cout << "tail(r) = ";
  printList(tail(r));
  std::cout << "init(r) = ";
  printList(init(r));

  auto ls = makeList(makeList(1, 2, 3), makeList(4, 5, 6));
  auto lc = concat(ls);
  printList(lc);

  auto duplicate = [](int x) { return cons<int>(x, cons<int>(x)); };
  auto dl = concatMap(duplicate, l);
  printList(dl);
}
