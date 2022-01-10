#include <iostream>
#include <utility>
#include <vector>

class Base {

protected:
  std::vector<double> const &vec1;
  std::vector<double> const &vec2;

public:
  Base(std::vector<double> const &v1, std::vector<double> const &v2)
      : vec1(v1), vec2(v2) {}

  std::pair<std::vector<double> const &, std::vector<double> const &> const
  getSequences() const {
    return std::pair<std::vector<double> const &, std::vector<double> const &>(
        vec1, vec2);
    // return result;
    // return std::make_pair(vec1, vec2);
  }
};

class Derived : public Base {
public:
  Derived(std::vector<double> const &v1, std::vector<double> const &v2)
      : Base{v1, v2} {}
};

class UseDerived {

  Derived &derived;

public:
  UseDerived(Derived &d) : derived(d) {}

  Derived &getDerived() { return derived; }
};