/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   hier 
 *
 ************************************************************************/

#ifndef included_pdat_CellVariable
#define included_pdat_CellVariable

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/hier/Variable.h"
#include "SAMRAI/tbox/Complex.h"

#include <string>

namespace SAMRAI {
namespace pdat {

/*!
 * Class CellVariable<DIM> is a templated variable class used to define
 * cell-centered quantities on an AMR mesh.   It is a subclass of
 * hier::Variable and is templated on the type of the underlying data
 * (e.g., double, int, bool, etc.).
 *
 * See header file for CellData<DIM> class for a more detailed
 * description of the data layout.
 *
 * @see pdat::CellData
 * @see pdat::CellDataFactory
 * @see hier::Variable
 */

template<class TYPE>
class CellVariable:public hier::Variable
{
public:
   /*!
    * @brief Create a cell-centered variable object with the given name and
    * depth (i.e., number of data values at each cell index location).
    * A default depth of one is provided.
    */
   explicit CellVariable(
      const tbox::Dimension& dim,
      const std::string& name,
      int depth = 1);

   /*!
    * @brief Virtual destructor for cell variable objects.
    */
   virtual ~CellVariable<TYPE>();

   /*!
    * @brief Return true indicating that cell data quantities will always
    * be treated as though fine values take precedence on coarse-fine
    * interfaces.  Note that this is really artificial since the cell
    * data index space matches the cell-centered index space for AMR
    * patches.  However, some value must be supplied for communication
    * operations.
    */
   bool fineBoundaryRepresentsVariable() const {
      return true;
   }

   /*!
    * @brief Return false indicating that cell data on a patch interior
    * does not exist on the patch boundary.
    */
   bool dataLivesOnPatchBorder() const {
      return false;
   }

   /*!
    * @brief Return the the depth (number of components).
    */
   int
   getDepth() const;

private:
   CellVariable(
      const CellVariable<TYPE>&);          // not implemented
   void
   operator = (
      const CellVariable<TYPE>&);               // not implemented
};

}
}

#ifdef INCLUDE_TEMPLATE_IMPLEMENTATION
#include "SAMRAI/pdat/CellVariable.C"
#endif

#endif
