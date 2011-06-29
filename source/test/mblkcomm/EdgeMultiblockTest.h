/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   AMR communication tests for edge-centered patch data 
 *
 ************************************************************************/

#ifndef included_EdgeMultiblockTest
#define included_EdgeMultiblockTest

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/tbox/Pointer.h"
#include "SAMRAI/tbox/Array.h"
#include "SAMRAI/hier/Box.h"
#include "SAMRAI/pdat/EdgeData.h"
#include "SAMRAI/tbox/Database.h"
#include "SAMRAI/hier/IntVector.h"
#include "SAMRAI/hier/Patch.h"
#include "PatchMultiblockTestStrategy.h"
#include "SAMRAI/hier/Variable.h"

using namespace SAMRAI;

/**
 * Class EdgeMultiblockTest provides routines to test communication operations
 * for edge-centered patch data on an AMR patch hierarchy.
 *
 * See PatchMultiblockTestStrategy header file comments for variable and
 * refinement input data description.
 */

class EdgeMultiblockTest:public PatchMultiblockTestStrategy
{
public:
   /**
    * The constructor initializes variable data arrays to zero length.
    */
   EdgeMultiblockTest(
      const string& object_name,
      const tbox::Dimension& dim,
      tbox::Pointer<tbox::Database> main_input_db,
      bool do_refine,
      bool do_coarsen,
      const string& refine_option);

   /**
    * Virtual destructor for EdgeMultiblockTest.
    */
   virtual ~EdgeMultiblockTest();

   /**
    * User-supplied boundary conditions.  Note that we do not implement
    * user-defined coarsen and refine operations.
    */
   void
   setPhysicalBoundaryConditions(
      hier::Patch& patch,
      const double time,
      const hier::IntVector& gcw_to_fill) const;

   void
   fillSingularityBoundaryConditions(
      hier::Patch& patch,
      const hier::PatchLevel& encon_level,
      const hier::Connector& dst_to_encon,
      const hier::Box& fill_box,
      const hier::BoundaryBox& bbox);

   /**
    * This function is called from the MultiblockTester constructor.  Its
    * purpose is to register variables used in the patch data test
    * and appropriate communication parameters (ghost cell widths,
    * coarsen/refine operations) with the MultiblockTester object, which
    * manages the variable storage.
    */
   void
   registerVariables(
      MultiblockTester* commtest);

   /**
    * Function for setting data on new patch in hierarchy.
    *
    * @param src_or_dst Flag set to 's' for source or 'd' for destination
    *        to indicate variables to set data for.
    */
   virtual void
   initializeDataOnPatch(
      hier::Patch& patch,
      const tbox::Pointer<hier::PatchHierarchy> hierarchy,
      const int level_number,
      const hier::BlockId& block_id,
      char src_or_dst);

   /**
    * Function for tagging cells on each patch to refine.
    */
   void
   tagCellsToRefine(
      hier::Patch& patch,
      const tbox::Pointer<hier::PatchHierarchy> hierarchy,
      int level_number,
      int tag_index);

   /**
    * Function for checking results of communication operations.
    */
   bool
   verifyResults(
      const hier::Patch& patch,
      const tbox::Pointer<hier::PatchHierarchy> hierarchy,
      const int level_number,
      const hier::BlockId& block_id);

   ///
   void
   postprocessRefine(
      hier::Patch& fine,
      const hier::Patch& coarse,
      const tbox::Pointer<hier::VariableContext>& context,
      const hier::Box& fine_box,
      const hier::IntVector& ratio) const;

private:
   /**
    * Function for reading test data from input file.
    */
   void
   readTestInput(
      tbox::Pointer<tbox::Database> db);

   /*
    * Object string identifier for error reporting
    */
   string d_object_name;

   const tbox::Dimension d_dim;

   string d_refine_option;
   int d_finest_level_number;

   tbox::Array<tbox::Pointer<hier::Variable> > d_variables;

};

#endif
