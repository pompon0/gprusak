#include "gtest/gtest.h"
#include <iostream>

#include "gprusak/utf.h"
#include "gprusak/log.h"

using namespace gprusak;

TEST(utf,simple) {
  StreamLogger _(std::cerr);
  str a8 = "Héllo ༃𐤋𐤊탄ཀ𐍊";
  auto [b32,b32_err] = utf8_to_utf32(a8);
  if(b32_err) FAIL() << "utf8_to_utf32(): " << b32_err->show();
  EXPECT_EQ(12,b32.size());
  auto [c8,c8_err] = utf32_to_utf8(b32);
  if(c8_err) FAIL() << "utf32_to_utf8(): " << c8_err->show();
  EXPECT_EQ(a8,c8);
}
