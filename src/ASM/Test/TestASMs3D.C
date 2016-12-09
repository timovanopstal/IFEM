//==============================================================================
//!
//! \file TestASMs3D.C
//!
//! \date Jul 14 2016
//!
//! \author Arne Morten Kvarving / SINTEF
//!
//! \brief Tests for driver for assembly of structured 3D spline FE models.
//!
//==============================================================================

#include "ASMs3D.h"
#include "IFEM.h"
#include "SIM3D.h"

#include "gtest/gtest.h"


class TestASMs3D :
  public testing::Test,
  public testing::WithParamInterface<int>
{
};


TEST_P(TestASMs3D, BoundaryNodes)
{
  SIM3D sim(1);
  ASSERT_TRUE(sim.read("src/ASM/LR/Test/refdata/boundary_nodes_3d.xinp"));
  sim.preprocess();

  std::stringstream str;
  str << "Face" << GetParam();
  int bcode = sim.getUniquePropertyCode(str.str(),0);
  std::vector<int> vec;
  sim.getBoundaryNodes(bcode, vec);

  ASSERT_EQ(vec.size(), 16U);
  auto it = vec.begin();
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (GetParam() == 1)
        ASSERT_EQ(*it++, 1+16*i+4*j);
      else if (GetParam() == 2)
        ASSERT_EQ(*it++, 4+16*i+4*j);
      else if (GetParam() == 3)
        ASSERT_EQ(*it++, 1+16*i+j);
      else if (GetParam() == 4)
        ASSERT_EQ(*it++, 13+16*i+j);
      else if (GetParam() == 5)
        ASSERT_EQ(*it++, 4*i+j+1);
      else if (GetParam() == 6)
        ASSERT_EQ(*it++, 49+4*i+j);
    }
  }

  vec.clear();
  sim.getPatch(1)->getBoundaryNodes(GetParam(), vec, 0, 2, true);
  it = vec.begin();
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int t = 0; t < 2; ++t) {
        if (GetParam() == 1)
          ASSERT_EQ(*it++, 1+16*i+4*j+t);
        else if (GetParam() == 2)
          ASSERT_EQ(*it++, 3+16*i+4*j+t);
        else if (GetParam() == 3)
          ASSERT_EQ(*it++, 1+16*i+j+4*t);
        else if (GetParam() == 4)
          ASSERT_EQ(*it++, 9+16*i+j+4*t);
        else if (GetParam() == 5)
          ASSERT_EQ(*it++, 4*i+j+1+16*t);
        else if (GetParam() == 6)
          ASSERT_EQ(*it++, 33+4*i+j+16*t);
      }
    }
  }
}


INSTANTIATE_TEST_CASE_P(TestASMs3D,
                        TestASMs3D,
                        testing::Values(1,2,3,4,5,6));
