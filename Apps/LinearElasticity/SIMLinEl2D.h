// $Id$
//==============================================================================
//!
//! \file SIMLinEl2D.h
//!
//! \date Feb 04 2010
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Solution driver for 2D NURBS-based linear elastic FEM analysis.
//!
//==============================================================================

#ifndef _SIM_LIN_EL_2D_H
#define _SIM_LIN_EL_2D_H

#include "SIM2D.h"
#include "SIMenums.h"

class Material;


/*!
  \brief Driver class for 2D isogeometric FEM analysis of elasticity problems.
  \details The class incapsulates data and methods for solving linear elasticity
  problems using NURBS-based finite elements. It reimplements the parse method
  of the parent class.
*/

class SIMLinEl2D : public SIM2D
{
public:
  //! \brief Default constructor.
  //! \param[in] form Problem formulation option
  SIMLinEl2D(int form = SIM::LINEAR);
  //! \brief The destructor frees the dynamically allocated material properties.
  virtual ~SIMLinEl2D();

protected:
  //! \brief Parses a data section from the input stream.
  //! \param[in] keyWord Keyword of current data section to read
  //! \param is The file stream to read from
  virtual bool parse(char* keyWord, std::istream& is);

  //! \brief Initializes material properties for integration of interior terms.
  //! \param[in] propInd Physical property index
  virtual bool initMaterial(size_t propInd);

  //! \brief Initializes for integration of Neumann terms for a given property.
  //! \param[in] propInd Physical property index
  virtual bool initNeumann(size_t propInd);

public:
  static bool planeStrain; //!< Plane strain/stress option
  static bool axiSymmetry; //!< Axisymmtry option

protected:
  std::vector<Material*> mVec; //!< Material data
};

#endif