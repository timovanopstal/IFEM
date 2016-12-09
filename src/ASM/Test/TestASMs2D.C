//==============================================================================
//!
//! \file TestASMs2D.C
//!
//! \date Jul 14 2016
//!
//! \author Arne Morten Kvarving / SINTEF
//!
//! \brief Tests for driver for assembly of structured 2D spline FE models.
//!
//==============================================================================

#include "ASMs2D.h"
#include "IFEM.h"
#include "SIM2D.h"

#include "gtest/gtest.h"


class TestASMs2D :
  public testing::Test,
  public testing::WithParamInterface<int>
{
};


TEST_P(TestASMs2D, BoundaryNodes)
{
  SIM2D sim(1);
  ASSERT_TRUE(sim.read("src/ASM/LR/Test/refdata/boundary_nodes.xinp"));
  sim.preprocess();

  std::stringstream str;
  str << "Edge" << GetParam();
  int bcode = sim.getUniquePropertyCode(str.str(),0);
  std::vector<int> vec;
  sim.getBoundaryNodes(bcode, vec);
  ASSERT_EQ(vec.size(), 4U);
  for (int i = 0; i < 4; ++i) {
    if (GetParam() == 1)
      ASSERT_EQ(vec[i], 4*i+1);
    else if (GetParam() == 2)
      ASSERT_EQ(vec[i], 4*(i+1));
    else if (GetParam() == 3)
      ASSERT_EQ(vec[i], i+1);
    else if (GetParam() == 4)
      ASSERT_EQ(vec[i], 13+i);
  }

  vec.clear();
  sim.getPatch(1)->getBoundaryNodes(GetParam(), vec, 0, 2, true);
  auto it = vec.begin();
  for (int i = 0; i < 4; ++i) {
    for (int t = 0; t < 2; ++t) {
      if (GetParam() == 1)
        ASSERT_EQ(*it++, 4*i+1+t);
      else if (GetParam() == 2)
        ASSERT_EQ(*it++, 4*(i+1)-1+t);
      else if (GetParam() == 3)
        ASSERT_EQ(*it++, i+1+4*t);
      else if (GetParam() == 4)
        ASSERT_EQ(*it++, 9+4*t+i);
    }
  }
}


INSTANTIATE_TEST_CASE_P(TestASMs2D,
                        TestASMs2D,
                        testing::Values(1, 2, 3, 4));
