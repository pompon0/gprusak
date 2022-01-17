#include "gtest/gtest.h"

#include "gprusak/errors.h"
#include "gprusak/log.h"

using namespace gprusak;
/*
struct ErrA : ErrSimple<ErrA> {};
struct ErrB : ErrSimple<ErrB> {};
struct ErrC : ErrSimple<ErrC> {};

Error<ErrA,ErrB>::Or<int> f(int x) {
  if(x>0) return x;
  return ErrA::New("yolo");
}

Error<ErrB,ErrC,ErrA> zz() {
  auto [x,e] = f(57);
  if(e) return std::move(e);
  return {};
}*/

struct ErrA : ErrText<ErrA> {};
struct ErrB {};

Error<>::Or<int> f(int x) {
  if(x>0) return x;
  return ErrA::New("hello");
}

TEST(A,B) {
  StreamLogger _(std::cerr);
  auto [x,e] = f(-56);
  if(e) {
    info("soso");
  }
  if(auto x = e.is<ErrA>()) {
    info("fff");
  }
}
