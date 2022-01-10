#include "scratch.h"

int main(int argc, char **argv) {
  std::cout << "hello world again" << std::endl;
  std::vector<double> data1{34.2, 22.1, 3.992, 4.5};
  std::vector<double> data2{14.2, 42.1, 23.992, 70.5};

  Derived d{data1, data2};
  UseDerived ud{d};

  auto usr = ud.getDerived();

  auto pr = usr.getSequences();

  auto f = [](std::vector<double> const &seq) {
    for (auto d : seq)
      std::cout << d << std::endl;
    std::cout << "_*_" << std::endl;
  };

  std::cout << "s1" << std::endl;
  f(pr.first);
  std::cout << "s2" << std::endl;
  f(pr.second);

  return 0;
}

// derived class uses base class getter to return protected member of base class