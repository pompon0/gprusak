#define DEBUG_MODE
#define VERBOSE

#include <iostream>

#include "gtest/gtest.h"
#include "gprusak/types.h"
#include "gprusak/number_theory.h"
#include "gprusak/log.h"

using namespace gprusak;

StreamLogger _(std::cerr);

TEST(inv,simple) {
  for(auto x : vec<uint64_t>{1,15,78934857,8745032764389431}) {
    info("x = %",x);
    EXPECT_EQ(1,uint8_t(x*inv<uint8_t>(x)));
    EXPECT_EQ(1,uint16_t(x*inv<uint16_t>(x)));
    EXPECT_EQ(1,uint32_t(x*inv<uint32_t>(x)));
    EXPECT_EQ(1,uint64_t(x*inv<uint64_t>(x)));
  }
}
