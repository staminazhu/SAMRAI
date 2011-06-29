/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   AdaptivePoisson class declaration 
 *
 ************************************************************************/
#ifndef included_AdaptivePoisson
#define included_AdaptivePoisson

#include "SAMRAI/solv/PoissonSpecifications.h"
#include "SAMRAI/solv/CellPoissonFACOps.h"
#include "SAMRAI/tbox/Dimension.h"
#include "PoissonSineSolution.h"
#include "PoissonPolynomialSolution.h"
#include "PoissonGaussianDiffcoefSolution.h"
#include "PoissonGaussianSolution.h"
#include "PoissonMultigaussianSolution.h"

#include <string>

#include "SAMRAI/tbox/Pointer.h"
#include "SAMRAI/tbox/Database.h"

/*
 * SAMRAI classes
 */
#include "SAMRAI/appu/VisItDataWriter.h"
#include "SAMRAI/appu/VisDerivedDataStrategy.h"
#include "SAMRAI/geom/CartesianCellDoubleConservativeLinearRefine.h"
#include "SAMRAI/geom/CartesianCellDoubleLinearRefine.h"
#include "SAMRAI/geom/CartesianCellDoubleWeightedAverage.h"
#include "SAMRAI/geom/CartesianSideDoubleWeightedAverage.h"
#include "SAMRAI/hier/Box.h"
#include "SAMRAI/hier/CoarseFineBoundary.h"
#include "SAMRAI/hier/Patch.h"
#include "SAMRAI/hier/PatchHierarchy.h"
#include "SAMRAI/hier/PatchLevel.h"
#include "SAMRAI/hier/IntVector.h"
#include "SAMRAI/mesh/StandardTagAndInitStrategy.h"
#include "SAMRAI/pdat/CellDoubleConstantRefine.h"
#include "SAMRAI/pdat/CellVariable.h"
#include "SAMRAI/pdat/SideVariable.h"
#include "SAMRAI/pdat/FaceVariable.h"
#include "SAMRAI/pdat/NodeVariable.h"
#include "SAMRAI/pdat/OutersideVariable.h"
#include "SAMRAI/solv/CartesianRobinBcHelper.h"
#include "SAMRAI/solv/FACPreconditioner.h"
#include "SAMRAI/solv/GhostCellRobinBcCoefs.h"
#include "SAMRAI/solv/RobinBcCoefStrategy.h"
#include "SAMRAI/solv/SAMRAIVectorReal.h"
#include "SAMRAI/xfer/RefinePatchStrategy.h"

using namespace SAMRAI;

/*!
 * @brief Class to solve Poisson's equation on a SAMR grid.
 *
 * This class tests the FAC solver class solving
 * Poisson's equation on a SAMR grid.
 *
 * This class inherits and implements virtual functions from
 * mesh::StandardTagAndInitStrategy to initialize data
 * on the SAMR grid.
 */
class AdaptivePoisson:
   public mesh::StandardTagAndInitStrategy,
   public appu::VisDerivedDataStrategy
{

public:
   /*!
    * @brief Constructor.
    *
    * Requirements:
    * - the referenced objects
    *
    * Actions:
    * - Set up private member data
    *
    * If you want standard output and logging,
    * pass in valid pointers for those streams.
    */
   AdaptivePoisson(
      const std::string& object_name,
      const tbox::Dimension& dim,
      tbox::Database& database,
      /*! Standard output stream */ std::ostream* out_stream = NULL,
      /*! Log output stream */ std::ostream* log_stream = NULL);

   //@{ @name mesh::StandardTagAndInitStrategy virtuals

public:
   /*!
    * @brief Allocate and initialize data for a new level
    * in the patch hierarchy.
    *
    * This is where you implement the code for initialize data on the
    * grid.  Nevermind when it is called or where in the program that
    * happens.  All the information you need to initialize the grid
    * are in the arguments.
    *
    * @see mesh::StandardTagAndInitStrategy::initializeLevelData()
    */
   virtual void
   initializeLevelData(
      /*! Hierarchy to initialize */
      const tbox::Pointer<hier::PatchHierarchy> hierarchy,
      /*! Level to initialize */
      const int level_number,
      const double init_data_time,
      const bool can_be_refined,
      /*! Whether level is being introduced for the first time */
      const bool initial_time,
      /*! Level to copy data from */
      const tbox::Pointer<hier::PatchLevel> old_level =
         tbox::Pointer<hier::PatchLevel>((0)),
      /*! Whether data on new patch needs to be allocated */
      const bool allocate_data = true);

   virtual void
   resetHierarchyConfiguration(
      /*! New hierarchy */
      tbox::Pointer<hier::PatchHierarchy> new_hierarchy,
      /*! Coarsest level */ int coarsest_level,
      /*! Finest level */ int finest_level);

   virtual void
   applyGradientDetector(
      const tbox::Pointer<hier::PatchHierarchy> hierarchy,
      const int level_number,
      const double error_data_time,
      const int tag_index,
      const bool initial_time,
      const bool uses_richardson_extrapolation);

   //@}

#if 0
   //@{
   /*!
    * @name Functions inherited from solv::CellPoissonFACOps
    */
   virtual void
   postprocessOneCycle(
      int iteration_num,
      const solv::SAMRAIVectorReal<double>& current_soln,
      const solv::SAMRAIVectorReal<double>& residual);
   //@}
#endif

   //@{ @name appu::VisDerivedDataStrategy virtuals

   virtual bool
   packDerivedDataIntoDoubleBuffer(
      double* buffer,
      const hier::Patch& patch,
      const hier::Box& region,
      const std::string& variable_name,
      int depth_id) const;

   //@}

public:
   /*!
    * @brief Solve using FAC solver.
    *
    * Set up the linear algebra problem and use a
    * solv::FACPreconditioner object to solve it.
    *
    * @param hierarchy the hierarchy to solve on
    * @param max_cycle max number of FAC cycles to use
    * @param pre_sweeps number of presmoothing sweeps to use
    * @param pre_sweeps number of postsmoothing sweeps to use
    * @param initial_u how to set the initial guess for u.
    *       A std::string is used so the option "random" can be
    *       used.  If "random" is not used, set the std::string
    *       to a floating point number.
    */
   int
   solvePoisson(
      tbox::Pointer<hier::PatchHierarchy> hierarchy,
      int max_cycles = 10,
      double residual_tol = 1e-6,
      int pre_sweeps = 5,
      int post_sweeps = 5,
      std::string initial_u = std::string("0.0"));

#ifdef HAVE_HDF5
   /*!
    * @brief Tell a plotter which data to write for this class.
    */
   int
   registerVariablesWithPlotter(
      appu::VisItDataWriter& visit_writer);
#endif

   /*!
    * @brief Compute the error of the current solution.
    *
    * Compute the @f$L_2@f$ and @f$L_\infty@f$ norms of the error,
    * for each level and over all levels.
    */
   int
   computeError(
      /*! hierarchy */ const hier::PatchHierarchy& hierarchy,
      /*! L2 norm */ double* l2norm,
      /*! L-inf norm */ double* linorm,
      /*! L2 norm on each level */ tbox::Array<double>& l2norms,
      /*! L-inf norm on each level */ tbox::Array<double>& linorms) const;

   /*!
    * @brief Compute error estimator (for adaption or plotting).
    *
    * Computes in the box defined by @c estimate_data.
    */
   void
   computeAdaptionEstimate(
      pdat::CellData<double>& estimate_data,
      const pdat::CellData<double>& soln_cell_data) const;

private:
   std::string d_name;
   const tbox::Dimension d_dim;

   tbox::Pointer<hier::PatchHierarchy> d_hierarchy;

   //@{
   /*!
    * @name Major algorithm objects.
    */

   solv::CellPoissonFACOps d_fac_ops;

   solv::FACPreconditioner d_fac_preconditioner;

   //@}

   //@{

   /*!
    * @name Private state variables for solution.
    */

   /*!
    * @brief Context for persistent data.
    */
   hier::VariableContext d_context_persistent;

   /*!
    * @brief Context for scratch data.
    */
   hier::VariableContext d_context_scratch;

   /*!
    * @brief Diffusion coefficient.
    */
   pdat::SideVariable<double> d_diffcoef;

   /*!
    * @brief Flux.
    */
   pdat::SideVariable<double> d_flux;

   /*!
    * @brief Scalar solution of Poisson's equation.
    */
   pdat::CellVariable<double> d_scalar;

   /*!
    * @brief Source for Poisson's equation.
    */
   pdat::CellVariable<double> d_constant_source;

   /*!
    * @brief Linear source operator for linear system.
    */
   pdat::CellVariable<double> d_ccoef;

   /*!
    * @brief Right hand side for linear system.
    */
   pdat::CellVariable<double> d_rhs;

   /*!
    * @brief Exact solution.
    */
   pdat::CellVariable<double> d_exact;

   /*!
    * @brief Residual.
    */
   tbox::Pointer<pdat::CellVariable<double> > d_resid;

   /*!
    * @brief Vector weights.
    *
    * For cells not covered by a finer cell, the weight
    * is the volume.  For cells that are, the weight is zero.
    * This is used in computing norms on the AMR grid.
    */
   pdat::CellVariable<double> d_weight;

   /*!
    * @brief Saved variable-context index.
    *
    * Because we refer to them often, variable-context indices are saved.
    * They are initialized in the constructor and never change.
    * Each index represents a variable-context pair in this class.
    * Thus the indices are @em not independent state variables.
    * If we had to set them, we need the associated variable and
    * context (and the variable database which manages the mapping).
    * See the hier::VariableDatabase class for more into.
    */
   int d_scalar_persistent, d_diffcoef_persistent,
       d_constant_source_persistent, d_weight_persistent,
       d_exact_persistent, d_rhs_scratch, d_resid_scratch,
       d_flux_persistent, d_ccoef_persistent;

   //@}

   //@{
private:
   /*!
    * @name Output streams.
    */
   /*!
    * @brief Output stream pointer.
    *
    * If set to NULL, no output.
    */
   std::ostream* d_ostream;

   /*!
    * @brief Log stream pointer.
    *
    * If set to NULL, no logging.
    */
   std::ostream* d_lstream;
   //@}

   //@{
   /*!
    * @name Miscellaneous.
    */
   std::string d_problem_name;
   //! @brief Poisson equation specifications.
   solv::PoissonSpecifications d_sps;
   //! @brief Things specific to the sinusoid solution
   PoissonSineSolution d_sine_solution;
   //! @brief Things specific to the Gaussian solution
   PoissonGaussianSolution d_gaussian_solution;
   //! @brief Things specific to the multi-Gaussian solution
   PoissonMultigaussianSolution d_multigaussian_solution;
   //! @brief Things specific to the polynomial solution
   PoissonPolynomialSolution d_polynomial_solution;
   //! @brief Things specific to the Gaussian coefficient solution
   PoissonGaussianDiffcoefSolution d_gaussian_diffcoef_solution;
   /*!
    * @brief Generic xfer::RefinePatchStrategy implementation for Robin bc.
    */
   solv::CartesianRobinBcHelper d_robin_refine_patch;
   /*!
    * @brief Physical bc coefficient strategy selecting one of the solutions'.
    */
   solv::RobinBcCoefStrategy* d_physical_bc_coef;
   //@}

   double d_adaption_threshold;

   int d_finest_plot_level;

   //@{
private:
   /*!
    * @name Objects to help debugging.
    */
#ifdef HAVE_HDF5
   tbox::Pointer<appu::VisItDataWriter> d_visit_writer;
#endif
   int d_finest_dbg_plot_ln;
   //@}

};

#endif  // included_AdaptivePoisson
