/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   hier 
 *
 ************************************************************************/

#ifndef included_pdat_OuterfaceGeometry
#define included_pdat_OuterfaceGeometry

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/pdat/FaceOverlap.h"
#include "SAMRAI/hier/Box.h"
#include "SAMRAI/hier/BoxGeometry.h"
#include "SAMRAI/hier/BoxOverlap.h"
#include "SAMRAI/hier/IntVector.h"
#include "SAMRAI/tbox/Pointer.h"

namespace SAMRAI {
namespace pdat {

class FaceGeometry;

/*!
 * Class OuterfaceGeometry manages the mapping between the AMR index
 * and the outerface geometry index space.  It is a subclass of
 * hier::BoxGeometry and it computes intersections between outerface
 * box geometries and face or outerface box geometries for communication
 * operations.
 *
 * See header file for OuterfaceData<DIM> class for a more detailed
 * description of the data layout.
 *
 * @see hier::BoxGeometry
 * @see pdat::FaceGeometry
 * @see pdat::FaceOverlap
 */

class OuterfaceGeometry:public hier::BoxGeometry
{
public:
   /*!
    * The BoxOverlap implemenation for this geometry.
    */
   typedef FaceOverlap Overlap;

   /*!
    * @brief Construct an outerface geometry object given an AMR index
    * space box and ghost cell width.
    */
   explicit OuterfaceGeometry(
      const hier::Box& box,
      const hier::IntVector& ghosts);

   /*!
    * @brief The virtual destructor does nothing interesting.
    */
   virtual ~OuterfaceGeometry();

   /*!
    * @brief Compute the overlap in face-centered index space on the
    * boundaries of the source box geometry and the destination box geometry.
    */
   virtual tbox::Pointer<hier::BoxOverlap>
   calculateOverlap(
      const hier::BoxGeometry& dst_geometry,
      const hier::BoxGeometry& src_geometry,
      const hier::Box& src_mask,
      const hier::Box& fill_box,
      const bool overwrite_interior,
      const hier::Transformation& transformation,
      const bool retry,
      const hier::BoxList& dst_restrict_boxes = hier::BoxList()) const;

   /*!
    * @brief Set up a FaceOverlap object based on the given boxes and the
    * transformation
    */
   virtual tbox::Pointer<hier::BoxOverlap>
   setUpOverlap(
      const hier::BoxList& boxes,
      const hier::Transformation& transformation) const;

   /*!
    * @brief Return the box for this outerface box geometry object.
    */
   const hier::Box&
   getBox() const;

   /*!
    * @brief Return the ghost cell width for this outerface box geometry object.
    */
   const hier::IntVector&
   getGhosts() const;

private:
   /**
    * Function doOverlap() is the function that computes the overlap
    * between the source and destination objects, where the source
    * has outerface geometry and the destination face geometry.
    */
   static tbox::Pointer<hier::BoxOverlap>
   doOverlap(
      const FaceGeometry& dst_geometry,
      const OuterfaceGeometry& src_geometry,
      const hier::Box& src_mask,
      const hier::Box& fill_box,
      const bool overwrite_interior,
      const hier::Transformation& transformation,
      const hier::BoxList& dst_restrict_boxes);

   OuterfaceGeometry(
      const OuterfaceGeometry&);                // not implemented
   void
   operator = (
      const OuterfaceGeometry&);                    // not implemented

   hier::Box d_box;
   hier::IntVector d_ghosts;

};

}
}
#ifdef SAMRAI_INLINE
#include "SAMRAI/pdat/OuterfaceGeometry.I"
#endif
#endif
