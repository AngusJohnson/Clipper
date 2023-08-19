#include <gtest/gtest.h>
#include "clipper2/clipper.h"

using namespace Clipper2Lib;

TEST(Clipper2Tests, TestRectClip)
{
  Paths64 sub, clp, sol;
  Rect64 rect = Rect64(100, 100, 700, 500);
  clp.push_back(rect.AsPath());

  sub.push_back(MakePath({ 100,100, 700,100, 700,500, 100,500 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(Area(sol) == Area(sub));

  sub.clear();
  sub.push_back(MakePath({ 110,110, 700,100, 700,500, 100,500 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(Area(sol) == Area(sub));

  sub.clear();
  sub.push_back(MakePath({ 90,90, 700,100, 700,500, 100,500 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(Area(sol) == Area(clp));

  sub.clear();
  sub.push_back(MakePath({ 110,110, 690,110, 690,490, 110,490 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(Area(sol) == Area(sub));

  sub.clear();
  clp.clear();
  rect = Rect64(390, 290, 410, 310);
  clp.push_back(rect.AsPath());
  sub.push_back(MakePath({ 410,290, 500,290, 500,310, 410,310 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(sol.empty());

  sub.clear();
  sub.push_back(MakePath({ 430,290, 470,330, 390,330 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(sol.empty());

  sub.clear();
  sub.push_back(MakePath({ 450,290, 480,330, 450,330 }));
  sol = RectClip(rect, sub);
  EXPECT_TRUE(sol.empty());

  sub.clear();
  sub.push_back(MakePath({ 208,66, 366,112, 402,303,
    234,332, 233,262, 243,140, 215,126, 40,172 }));
  rect = Rect64(237, 164, 322, 248);
  sol = RectClip(rect, sub);
  const auto solBounds = GetBounds(sol);
  EXPECT_EQ(solBounds.Width(), rect.Width());
  EXPECT_EQ(solBounds.Height(), rect.Height());

}

TEST(Clipper2Tests, TestRectClip2) //#597
{
  Clipper2Lib::Rect64 rect(54690, 0, 65628, 6000);
  Clipper2Lib::Paths64 subject {{{700000, 6000}, { 0, 6000 }, { 0, 5925 }, { 700000, 5925 }}};

  Clipper2Lib::Paths64 solution = Clipper2Lib::RectClip(rect, subject);

  //std::cout << solution << std::endl;
  EXPECT_TRUE(solution.size() == 1 && solution[0].size() == 4);
}


TEST(Clipper2Tests, TestRectClipWithHole) //#619
{
    Paths64 sub, clp, sol;
    Rect64 rect = Rect64(5, 5, 7, 7);
    clp.push_back(rect.AsPath());


    sub.push_back(MakePath({ 0, 0, 0, 16, 16, 16, 16, 0 })); // hole
    sol = RectClip(rect, sub);
    EXPECT_FLOAT_EQ(-4, Area(sol));

    sub.clear();
    sub.push_back(MakePath({ 0, 0, 16, 0, 16, 16, 0, 16 })); // poly
    sub.push_back(MakePath({ 2, 2, 2, 14, 14, 14, 14, 2 })); // hole
    sol = RectClip(rect, sub);

    EXPECT_EQ(2, sol.size());

    EXPECT_FLOAT_EQ(0, Area(sol)); // sol: one poly and one poly with same size
}