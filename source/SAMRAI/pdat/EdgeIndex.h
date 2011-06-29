/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   hier 
 *
 ************************************************************************/

#ifndef included_pdat_EdgeIndex
#define included_pdat_EdgeIndex

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/hier/IntVector.h"
#include "SAMRAI/hier/Index.h"

namespace SAMRAI {
namespace pdat {

/**
 * Class EdgeIndex implements a simple n-dimensional integer
 * vector for edge centered variables.  Edge indices contain an integer
 * index location in AMR index space along with the designated edge axis
 * (X=0, Y=1, or Z=2).  See the edge box geometry class for more information
 * about the mapping between the AMR index space and the edge indices.
 *
 * @see hier::Index
 * @see pdat::EdgeData
 * @see pdat::EdgeGeometry
 * @see pdat::EdgeIterator
 */

class EdgeIndex:public hier::Index
{
public:
   /**
    * The default constructor for a edge index creates an uninitialized index.
    */
   explicit EdgeIndex(
      const tbox::Dimension& dim);

   /**
    * Construct a edge index from a regular index, axis, and edge.  The axis
    * can be one of EdgeIndex::X (0), EdgeIndex::Y (1), or
    * EdgeIndex::Z (2). The edge argument can be one of the constants
    * EdgeIndex::LowerLeft (0), EdgeIndex::LowerRight(1),
    * EdgeIndex::UpperLeft (2) or EdgeIndex::UpperRight(3).
    */
   explicit EdgeIndex(
      const hier::Index& rhs,
      const int axis,
      const int edge);

   /**
    * The copy constructor creates a edge index equal to the argument.
    */
   EdgeIndex(
      const EdgeIndex& rhs);

   /**
    * The assignment operator sets the edge index equal to the argument.
    */
   EdgeIndex&
   operator = (
      const EdgeIndex& rhs);

   /**
    * The edge index destructor does nothing interesting.
    */
   ~EdgeIndex();

   /**
    * Get the axis for which this edge index is defined (X=0, Y=1, Z=2).
    */
   int
   getAxis() const;

   /**
    * Set the edge axis (X=0, Y=1, Z=2).
    */
   void
   setAxis(
      const int axis);

   /**
    * For dimension 2, converts the edge index into the index on the
    * lower side (argument edge = 0) or the upper side (argument
    * edge = 1) of the edge.
    * For dimension 3, converts the edge index into the index on the lower
    * left (argument edge == 0), the lower right (argument edge == 1),
    * the upper left (argument edge == 2), or the upper right
    * (argument edge == 3) of thw edge.
    */
   hier::Index
   toCell(
      const int edge) const;

   /**
    * Plus-equals operator for a edge index and an integer vector.
    */
   EdgeIndex&
   operator += (
      const hier::IntVector& rhs);

   /**
    * Plus operator for a edge index and an integer vector.
    */
   EdgeIndex
   operator + (
      const hier::IntVector& rhs) const;

   /**
    * Plus-equals operator for a edge index and an integer.
    */
   EdgeIndex&
   operator += (
      const int rhs);

   /**
    * Plus operator for a edge index and an integer.
    */
   EdgeIndex
   operator + (
      const int rhs) const;

   /**
    * Minus-equals operator for a edge index and an integer vector.
    */
   EdgeIndex&
   operator -= (
      const hier::IntVector& rhs);

   /**
    * Minus operator for a edge index and an integer vector.
    */
   EdgeIndex
   operator - (
      const hier::IntVector& rhs) const;

   /**
    * Minus-equals operator for a edge index and an integer.
    */
   EdgeIndex&
   operator -= (
      const int rhs);

   /**
    * Minus operator for a edge index and an integer.
    */
   EdgeIndex
   operator - (
      const int rhs) const;

   /**
    * Times-equals operator for a edge index and an integer vector.
    */
   EdgeIndex&
   operator *= (
      const hier::IntVector& rhs);

   /**
    * Times operator for a edge index and an integer vector.
    */
   EdgeIndex
   operator * (
      const hier::IntVector& rhs) const;

   /**
    * Times-equals operator for a edge index and an integer.
    */
   EdgeIndex&
   operator *= (
      const int rhs);

   /**
    * Times operator for a edge index and an integer.
    */
   EdgeIndex
   operator * (
      const int rhs) const;

   /**
    * Returns true if two edge index objects are equal.  All components
    * and the corresponding edge axes must be the same for equality.
    */
   bool
   operator == (
      const EdgeIndex& rhs) const;

   /**
    * Returns true if two edge index objects are not equal.  Any of
    * the components or axes may be different for inequality.
    */
   bool
   operator != (
      const EdgeIndex& rhs) const;

   enum {

      X = 0,
      Y = 1,
      Z = 2,
      Lower = 0,
      Upper = 1,
      LowerLeft = 0,
      LowerRight = 1,
      UpperLeft = 2,
      UpperRight = 3

   };

private:
   int d_axis;
};

}
}
#ifdef SAMRAI_INLINE
#include "SAMRAI/pdat/EdgeIndex.I"
#endif
#endif
