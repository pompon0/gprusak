#include "gtest/gtest.h"

#define DEBUG_MODE
#define VERBOSE

#include <iostream>

#include "gprusak/ctx.h"
#include "gprusak/group.h"

using namespace gprusak;
StreamLogger _(std::cout);

TEST(ctx,simple) {
  auto ctx1 = Ctx::background();
  auto ctx2 = ctx1.with_cancel();
  auto ctx3 = ctx1.with_cancel();
  ctx2.cancel();
  EXPECT_FALSE(ctx1.done()); 
  EXPECT_TRUE(ctx2.done()); 
  EXPECT_FALSE(ctx3.done()); 
}

TEST(ctx,group) {
  auto ctx = Ctx::background().with_deadline(absl::UnixEpoch());
  auto err = Group::Run(ctx,[](Ctx ctx, Group &g) -> Error<> {
    g.spawn([&](Ctx ctx) -> Error<> {
      return {};
    });
    return {}; 
  });
  if(err) FAIL() << err->show();
}
