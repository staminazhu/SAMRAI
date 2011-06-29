/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   IndexDataFactory
 *
 ************************************************************************/

#ifndef included_pdat_IndexDataFactory
#define included_pdat_IndexDataFactory

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/hier/Box.h"
#include "SAMRAI/hier/IntVector.h"
#include "SAMRAI/hier/Patch.h"
#include "SAMRAI/hier/PatchData.h"
#include "SAMRAI/hier/PatchDataFactory.h"
#include "SAMRAI/tbox/Pointer.h"

namespace SAMRAI {
namespace pdat {

/**
 * Class IndexDataFactory<DIM> is the factory class used to allocate
 * new instances of IndexData<DIM> objects.  It is a subclass of the
 * hier::PatchDataFactory base class.
 *
 * @see pdat::IndexData
 * @see pdat::IndexVariable
 * @see hier::PatchDataFactory
 */

template<class TYPE, class BOX_GEOMETRY>
class IndexDataFactory:public hier::PatchDataFactory
{
public:
   /**
    * The default constructor for the IndexDataFactory<DIM> class.
    * The ghost cell width argument gives the default width for all
    * irregular data objects created with this factory.
    */
   explicit IndexDataFactory(
      const hier::IntVector& ghosts);

   /**
    * Virtual destructor for the irregular data factory class.
    */
   virtual ~IndexDataFactory<TYPE, BOX_GEOMETRY>();

   /**
    * @brief Abstract virtual function to clone a patch data factory.
    *
    * This will return a new instantiation of the abstract factory
    * with the same properties.  The properties of the cloned factory
    * can then be changed without modifying the original.
    *
    * @param ghosts default ghost cell width for concrete classes created from
    * the factory.
    */
   virtual tbox::Pointer<hier::PatchDataFactory>
   cloneFactory(
      const hier::IntVector& ghosts);

   /**
    * Virtual factory function to allocate a concrete index data object.
    * The default information about the object (e.g., ghost cell width) is
    * taken from the factory.
    */
   virtual tbox::Pointer<hier::PatchData>
   allocate(
      const hier::Patch& patch) const;

   /**
    * Allocate the box geometry object associated with the patch data.
    * This information will be used in the computation of intersections
    * and data dependencies between objects.
    */
   virtual tbox::Pointer<hier::BoxGeometry>
   getBoxGeometry(
      const hier::Box& box) const;

   /**
    * Calculate the amount of memory needed to store the irregular data
    * object, including object data but not dynamically allocated data.
    * Because the irregular data list can grow and shrink, it would be
    * impossible to estimate the necessary amount of memory.  Instead,
    * dynamic data is allocated via the standard new/free mechanisms.
    */
   virtual size_t
   getSizeOfMemory(
      const hier::Box& box) const;

   /**
    * Return a boolean true value indicating that the index data quantities will always
    * be treated as though fine values represent them on coarse-fine interfaces.
    * See the IndexVariable<DIM> class header file for more information.
    */
   bool fineBoundaryRepresentsVariable() const {
      return true;
   }

   /**
    * Return false since the index data index space matches the cell-centered
    * index space for AMR patches.  Thus, index data does not live on patch borders.
    */
   bool dataLivesOnPatchBorder() const {
      return false;
   }

   /**
    * Return whether it is valid to copy this IndexDataFactory to the
    * supplied destination patch data factory.  It will return true if
    * dst_pdf is an IndexDataFactory of the same type and dimension,
    * false otherwise.
    */
   bool
   validCopyTo(
      const tbox::Pointer<hier::PatchDataFactory>& dst_pdf) const;

private:
   IndexDataFactory(
      const IndexDataFactory<TYPE, BOX_GEOMETRY>&);
   void
   operator = (
      const IndexDataFactory<TYPE, BOX_GEOMETRY>&);

};

}
}

#ifdef INCLUDE_TEMPLATE_IMPLEMENTATION
#include "SAMRAI/pdat/IndexDataFactory.C"
#endif

#endif
