// $Id: ASMs3D.h,v 1.21 2011-01-28 13:34:28 kmo Exp $
//==============================================================================
//!
//! \file ASMs3D.h
//!
//! \date Sep 20 2009
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Driver for assembly of structured 3D spline FE models.
//!
//==============================================================================

#ifndef _ASM_S3D_H
#define _ASM_S3D_H

#include "ASMstruct.h"

namespace Go {
  class SplineVolume;
  class BasisDerivs;
  class BasisDerivs2;
}


/*!
  \brief Driver for assembly of structured 3D spline FE models.
  \details This class contains methods common for structured 3D spline patches.
*/

class ASMs3D : public ASMstruct
{
  //! \brief Struct for nodal point data.
  struct IJK
  {
    int I; //!< Index in first parameter direction
    int J; //!< Index in second parameter direction
    int K; //!< Index in third parameter direction
  };

  //! \brief Struct for edge node definitions.
  struct Edge
  {
    int icnod; //!< Global node number of first interior point along the edge
    int incr;  //!< Increment in the global numbering along the edge

    //! \brief Default constructor.
    Edge() { icnod = incr = 0; }
    //! \brief Returns \a icnod which then is incremented.
    int next();
  };

  //! \brief Struct for face node definitions.
  struct Face
  {
    int isnod; //!< Global node number of the first interior point on the face
    int incrI; //!< Increment in global numbering in the I-direction on the face
    int incrJ; //!< Increment in global numbering in the J-direction on the face
    int nnodI; //!< Number of nodes in the local I-direction on the face
    int indxI; //!< Running node index in the local I-direction

    //! \brief Default constructor.
    Face() { isnod = incrI = incrJ = nnodI = 0; indxI = 1; }
    //! \brief Returns \a isnod which then is incremented.
    int next();
  };

public:
  //! \brief Struct with data for definition of global node numbers of a patch.
  struct BlockNodes
  {
    int  ibnod[8];  //!< Vertex nodes
    Edge edges[12]; //!< Edge nodes
    Face faces[6];  //!< Face nodes
    int  iinod;     //!< Global node number of the first interior node
    int  inc[3];    //!< Increment in global node numbering in each direction
    int  nnodI;     //!< Number of nodes in parameter direction I
    int  nnodJ;     //!< Number of nodes in parameter direction J
    int  indxI;     //!< Running node index in the local I-direction
    int  indxJ;     //!< Running node index in the local J-direction

    //! \brief Default constructor.
    BlockNodes() { iinod = inc[0] = inc[1] = inc[2] = 0; indxI = indxJ = 1; }
    //! \brief Returns \a iinod which then is incremented.
    int next();
  };

  //! \brief Constructor creating an instance by reading the given file.
  ASMs3D(const char* fileName, bool checkRHS = false, unsigned char n_f = 3);
  //! \brief Constructor creating an instance by reading the given input stream.
  ASMs3D(std::istream& is, bool checkRHS = false, unsigned char n_f = 3);
  //! \brief Default constructor creating an empty patch.
  ASMs3D(unsigned char n_f = 3) : ASMstruct(3,3,n_f) { svol = 0; }
  //! \brief Empty destructor.
  virtual ~ASMs3D() {}


  // Methods for model generation
  // ============================

  //! \brief Generates the finite element topology data for the patch.
  //! \details The data generated are the element-to-node connectivity array,
  //! the node-to-IJK-index array, as well as global node and element numbers.
  virtual bool generateFEMTopology();

  //! \brief Clears the contents of the patch, making it empty.
  virtual void clear();

  //! \brief Returns the global coordinates for the given node.
  //! \param[in] inod 1-based node index local to current patch
  virtual Vec3 getCoord(size_t inod) const;

  //! \brief Creates an instance by reading the given input stream, \a is.
  bool read(std::istream& is);
  //! \brief Writes the geometry of the SplineVolume object to the given stream.
  bool write(std::ostream& os) const;

  //! \brief Assigns new global node numbers for all nodes of the patch.
  //! \param nodes Object with global nodes numbers to assign to this patch
  //! \param[in] basis Which basis to assign node numbers for in mixed methods
  //!
  //! \details The global node numbers generated by \a generateFEMTopology are
  //! non-unique in the sense that a node that is shared by two (or more)
  //! patches along a common interface has a different number in each patch.
  //! This method therefore assigns a new global number to each node in the
  //! patch. The data provided through the \a nodes argument is sufficient
  //! to determine the unique global number under the assumption that they
  //! are ordered in the sequence determined by the local orientation of the
  //! patch, its faces and edges.
  bool assignNodeNumbers(BlockNodes& nodes, int basis = 0);

  //! \brief Check that the patch is modelled in a right-hand-side system.
  //! \details If it isn't, the w-parameter direction is swapped.
  bool checkRightHandSystem();

  //! \brief Refine the parametrization by inserting extra knots.
  //! \param[in] dir Parameter direction to refine
  //! \param[in] xi Relative positions of added knots in each existing knot span
  bool refine(int dir, const RealArray& xi);
  //! \brief Refine the parametrization by inserting extra knots uniformly.
  //! \param[in] dir Parameter direction to refine
  //! \param[in] nInsert Number of extra knots to insert in each knot-span
  bool uniformRefine(int dir, int nInsert);
  //! \brief Raise the order of the SplineVolume object for this patch.
  //! \param[in] ru Number of times to raise the order in u-direction
  //! \param[in] rv Number of times to raise the order in v-direction
  //! \param[in] rw Number of times to raise the order in w-direction
  bool raiseOrder(int ru, int rv, int rw);


  // Various methods for preprocessing of boundary conditions
  // ========================================================

  //! \brief Makes two opposite boundary faces periodic.
  //! \param[in] dir Parameter direction defining the periodic faces
  void closeFaces(int dir);

  //! \brief Constrains all DOFs on a given boundary face.
  //! \param[in] dir Parameter direction defining the face to constrain
  //! \param[in] dof Which DOFs to constrain at each node on the face
  //! \param[in] code Inhomogeneous dirichlet condition code
  void constrainFace(int dir, int dof = 123, int code = 0);

  //! \brief Constrains all DOFs on a given boundary edge.
  //! \param[in] lEdge Local index [1,12] of the edge to constrain
  //! \param[in] dof Which DOFs to constrain at each node on the edge
  //! \param[in] code Inhomogeneous dirichlet condition code
  void constrainEdge(int lEdge, int dof = 123, int code = 0);

  //! \brief Constrains all DOFs along a line on a given boundary face.
  //! \param[in] fdir Parameter direction defining the face to constrain
  //! \param[in] ldir Parameter direction defining the line to constrain
  //! \param[in] xi Parameter value defining the line to constrain
  //! \param[in] dof Which DOFs to constrain at each node along the line
  //! \param[in] code Inhomogeneous dirichlet condition code
  //!
  //! \details The parameter \a xi has to be in the domain [0.0,1.0], where
  //! 0.0 means the beginning of the domain and 1.0 means the end. The line to
  //! constrain goes along the parameter direction \a ldir in the face with
  //! with normal in parameter direction \a fdir, and positioned along the third
  //! parameter direction as indicated by \a xi. The actual value of \a xi
  //! is converted to the integer value closest to \a xi*n, where \a n is the
  //! number of nodes (control points) in that parameter direction.
  void constrainLine(int fdir, int ldir, double xi,
		     int dof = 123, int code = 0);

  //! \brief Constrains a corner node identified by the three parameter indices.
  //! \param[in] I Parameter index in u-direction
  //! \param[in] J Parameter index in v-direction
  //! \param[in] K Parameter index in w-direction
  //! \param[in] dof Which DOFs to constrain at the node
  //! \param[in] code Inhomogeneous dirichlet condition code
  //!
  //! \details The sign of the three indices is used to define whether we want
  //! the node at the beginning or the end of that parameter direction.
  //! The magnitude of the indices are not used.
  void constrainCorner(int I, int J, int K,
		       int dof = 123, int code = 0);
  //! \brief Constrains a node identified by three relative parameter values.
  //! \param[in] xi Parameter in u-direction
  //! \param[in] eta Parameter in v-direction
  //! \param[in] zeta Parameter in w-direction
  //! \param[in] dof Which DOFs to constrain at the node
  //! \param[in] code Inhomogeneous dirichlet condition code
  //!
  //! \details The parameter values have to be in the domain [0.0,1.0], where
  //! 0.0 means the beginning of the domain and 1.0 means the end. For values
  //! in between, the actual index is taken as the integer value closest to
  //! \a r*n, where \a r denotes the given relative parameter value,
  //! and \a n is the number of nodes along that parameter direction.
  void constrainNode(double xi, double eta, double zeta,
		     int dof = 123, int code = 0);

  //! \brief Connects all matching nodes on two adjacent boundary faces.
  //! \param[in] face Local face index of this patch, in range [1,6]
  //! \param neighbor The neighbor patch
  //! \param[in] nface Local face index of neighbor patch, in range [1,6]
  //! \param[in] norient Relative face orientation flag (see below)
  //!
  //! \details The face orientation flag \a norient must be in range [0,7].
  //! When interpreted as a binary number, its 3 digits are decoded as follows:
  //! - left digit = 1: The u and v parameters of the neighbor face are swapped
  //! - middle digit = 1: Parameter \a u in neighbor patch face is reversed
  //! - right digit = 1: Parameter \a v in neighbor patch face is reversed
  bool connectPatch(int face, ASMs3D& neighbor, int nface, int norient = 0);


  // Methods for integration of finite element quantities.
  // These are the main computational methods of the ASM class hierarchy.
  // ====================================================================

  //! \brief Evaluates an integral over the interior patch domain.
  //! \param integrand Object with problem-specific data and methods
  //! \param glbInt The integrated quantity
  //! \param[in] time Parameters for nonlinear/time-dependent simulations
  //! \param locInt Vector of element-wise contributions to \a glbInt
  virtual bool integrate(Integrand& integrand,
			 GlobalIntegral& glbInt, const TimeDomain& time,
			 const LintegralVec& locInt = LintegralVec());

  //! \brief Evaluates a boundary integral over a patch face.
  //! \param integrand Object with problem-specific data and methods
  //! \param[in] lIndex Local index [1,6] of the boundary face
  //! \param glbInt The integrated quantity
  //! \param[in] time Parameters for nonlinear/time-dependent simulations
  //! \param locInt Vector of element-wise contributions to \a glbInt
  virtual bool integrate(Integrand& integrand, int lIndex,
			 GlobalIntegral& glbInt, const TimeDomain& time,
			 const LintegralVec& locInt = LintegralVec());

  //! \brief Evaluates a boundary integral over a patch edge.
  //! \param integrand Object with problem-specific data and methods
  //! \param[in] lEdge Local index [1,12] of the patch edge
  //! \param glbInt The integrated quantity
  //! \param[in] time Parameters for nonlinear/time-dependent simulations
  virtual bool integrateEdge(Integrand& integrand, int lEdge,
			     GlobalIntegral& glbInt, const TimeDomain& time);


  // Post-processing methods
  // =======================

  //! \brief Creates a hexahedron element model of this patch for visualization.
  //! \param[out] grid The generated hexahedron grid
  //! \param[in] npe Number of visualization nodes over each knot span
  //! \note The number of element nodes must be set in \a grid on input.
  virtual bool tesselate(ElementBlock& grid, const int* npe) const;

  //! \brief Evaluates the primary solution field at all visualization points.
  //! \param[out] sField Solution field
  //! \param[in] locSol Solution vector in DOF-order
  //! \param[in] npe Number of visualization nodes over each knot span
  virtual bool evalSolution(Matrix& sField, const Vector& locSol,
			    const int* npe) const;

  //! \brief Evaluates the secondary solution field at all visualization points.
  //! \param[out] sField Solution field
  //! \param[in] integrand Object with problem-specific data and methods
  //! \param[in] npe Number of visualization nodes over each knot span
  //!
  //! \details If \a npe is NULL, the solution is evaluated at the Greville
  //! points and then projected onto the spline basis to obtain the control
  //! point values, which then are returned through \a sField.
  virtual bool evalSolution(Matrix& sField, const Integrand& integrand,
			    const int* npe = 0) const;

  //! \brief Projects the secondary solution field onto the primary basis.
  //! \param[in] integrand Object with problem-specific data and methods
  Go::SplineVolume* projectSolution(const Integrand& integrand) const;
  //! \brief Projects the secondary solution field onto the primary basis.
  virtual Go::GeomObject* evalSolution(const Integrand& integrand) const;

protected:

  // Internal utility methods
  // ========================

  //! \brief Evaluates the secondary solution field at the given points.
  //! \param[out] sField Solution field
  //! \param[in] integrand Object with problem-specific data and methods
  //! \param[in] gpar Parameter values of the result sampling points
  bool evalSolution(Matrix& sField, const Integrand& integrand,
		    const RealArray* gpar) const;

  //! \brief Calculates parameter values for visualization nodal points.
  //! \param[out] prm Parameter values in given direction for all points
  //! \param[in] dir Parameter direction (0,1,2)
  //! \param[in] nSegSpan Number of visualization segments over each knot-span
  virtual bool getGridParameters(RealArray& prm, int dir, int nSegSpan) const;

  //! \brief Calculates parameter values for the Greville points.
  //! \param[out] prm Parameter values in given direction for all points
  //! \param[in] dir Parameter direction (0,1,2)
  bool getGrevilleParameters(RealArray& prm, int dir) const;

  //! \brief Returns the volume in the parameter space for an element.
  //! \param[in] iel Element index
  double getParametricVolume(int iel) const;
  //! \brief Returns boundary face area in the parameter space for an element.
  //! \param[in] iel Element index
  //! \param[in] dir Local face index of the boundary face
  double getParametricArea(int iel, int dir) const;

  //! \brief Returns a matrix with nodal coordinates for an element.
  //! \param[in] iel Element index
  //! \param[out] X 3\f$\times\f$n-matrix, where \a n is the number of nodes
  //! in one element
  virtual bool getElementCoordinates(Matrix& X, int iel) const;
  //! \brief Returns a matrix with all nodal coordinates within the patch.
  //! \param[out] X 3\f$\times\f$n-matrix, where \a n is the number of nodes
  //! in the patch
  virtual void getNodalCoordinates(Matrix& X) const;

  //! \brief Returns the number of nodal points in each parameter direction.
  //! \param[out] n1 Number of nodes in first (u) direction
  //! \param[out] n2 Number of nodes in second (v) direction
  //! \param[out] n3 Number of nodes in third (w) direction
  //! \param[in] basis Which basis to return size parameters for (mixed methods)
  virtual bool getSize(int& n1, int& n2, int& n3, int basis = 0) const;

  //! \brief Establishes matrices with basis functions and 1st derivatives.
  static void extractBasis(const Go::BasisDerivs& spline,
			   Vector& N, Matrix& dNdu);
  //! \brief Establishes matrices with basis functions, 1st and 2nd derivatives.
  static void extractBasis(const Go::BasisDerivs2& spline,
			   Vector& N, Matrix& dNdu, Matrix3D& d2Ndu2);

  //! \brief Auxilliary function for computation of basis function indices.
  static void scatterInd(int n1, int n2, int n3, int p1, int p2, int p3,
			 const int* start, IntVec& index);

private:
  //! \brief Returns an index into the internal coefficient array for a node.
  //! \param[in] inod 0-based node index local to current patch
  int coeffInd(size_t inod) const;

protected:
  Go::SplineVolume* svol;    //!< Pointer to the actual spline volume object
  bool              swapW;   //!< Has the w-parameter direction been swapped?
  std::vector<IJK>  nodeInd; //!< IJK-triplets for the control points (nodes)
};

#endif
