/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright 
 * information, see COPYRIGHT and COPYING.LESSER. 
 *
 * Copyright:     (c) 1997-2011 Lawrence Livermore National Security, LLC
 * Description:   Support for coordinating multiple asynchronous communications 
 *
 ************************************************************************/
#ifndef included_tbox_AsyncCommStage
#define included_tbox_AsyncCommStage

#ifndef included_SAMRAI_config
#include "SAMRAI/SAMRAI_config.h"
#endif

#include "SAMRAI/tbox/SAMRAI_MPI.h"
#include "SAMRAI/tbox/Timer.h"

#include <vector>

namespace SAMRAI {
namespace tbox {

/*!
 * @brief Stage multiple non-blocking MPI communications so that codes
 * waiting for them to complete can advance as their respective MPI
 * requests are completed.
 *
 * An AsyncCommStage object manages multiple non-blocking MPI
 * communications carried out by AsyncCommStage::Member objects.
 * These classes factor out multiple non-blocking communication codes.
 *
 * By being staged together, Members can overlap their communication
 * waits conveniently.  The stage determines which Member's
 * communication requests are completed.  User code then follows up
 * the communication with whatever was waiting for that operation to
 * complete.
 *
 * The Member class defines the interfaces required to work with an
 * AsyncCommStage.  Communication operations can be a simple
 * non-blocking MPI communication or a complex sequence of MPI
 * communications and local computations.  See Member subclasses for
 * examples.
 *
 * Use this class when you:
 * - Have multiple communications, each performed
 *   by a different and independent AsyncCommStage::Member object.
 * - Want the multiple communications to proceed
 *   independently and asynchronously.
 *
 * Each AsyncCommStage is a registry of Member objects on the stage.
 * To overlap the Members' communications, the stage manages space for
 * their SAMRAI_MPI::Request's and SAMRAI_MPI::Status'es.  The
 * requests are allocated such that a single MPI_Waitany or
 * MPI_Waitsome represents all Members registered with the stage.
 * Thus the communications performed by the Member objects can
 * complete in the order allowed by the arrival of MPI messages.  The
 * exact order is NOT deterministic!
 *
 * To advance the communication operation of any of the allocated
 * Member objects, use advanceAny() or advanceSome().  In general,
 * advanceSome() has better performance than advanceAny() because it
 * gets around the "starvation" problem.  See the MPI documentation
 * for a discussion of starvation.
 *
 * This class supports communication and uses MPI for message passing.
 * If MPI is disabled, the job of this class disappears and the class
 * is effectively empty, except for managing the registration of
 * Member objects.  The public interfaces still remain so the class
 * can compile, but the implementations are trivial.
 */
class AsyncCommStage
{

public:
   /*!
    * @brief Optional object that can be attached to each Member,
    * for use in determining what to do after the Member completes
    * its communication.
    *
    * This class is empty because it serves only to be subclassed.
    *
    * @see Member::setHandler().
    */
   struct Handler {
      virtual ~Handler() {
      }
   };

   /*!
    * @brief Something on a stage, using MPI requests and statuses
    * provided by the stage.
    *
    * This class defines the interfaces required to participate
    * in a AsyncCommStage.  Subclasses should implement communication
    * operations using the MPI requests and statuses returned by
    * getRequestPointer() and getStatusPointer().
    */
   struct Member {
      /*!
       * @brief Default constructor, requiring a follow-up call to
       * attachStage() to properly stage the Member.
       */
      Member();
      /*!
       * @brief Initializing constructor.
       *
       * Same as default construction plus attachStage(), plus
       * setHandler().
       */
      Member(
         const size_t nreq,
         AsyncCommStage* stage,
         Handler* handler);
      /*!
       * @brief Destructor detach the Member from its stage.
       * Memory allocated by the stage to support the Member
       * will be recycled.
       */
      virtual ~Member();

      /*!
       * @brief Get the number of requests for the stage Member.
       *
       * This is the number used to initialize the object.  If the
       * Member has not been initialized to a stage, zero is returned.
       */
      size_t
      numberOfRequests() const;

      /*!
       * @brief Return the number of pending SAMRAI_MPI::Request
       * objects for the Member.
       */
      size_t
      numberOfPendingRequests() const;

      /*!
       * @brief Return whether the Member has some pending communication
       * requests.
       */
      bool
      hasPendingRequests() const;

      /*!
       * @brief Check whether entire operation is done.
       *
       * This should not just check the MPI requests--that is the job
       * for hasPendingRequests().  It should check the entire
       * communication operation (which may have follow-up
       * communications and computations).  If MPI communications are
       * completed but have not been followed through by a
       * completeCurrentOperation(), it should return false.
       */
      virtual bool
      isDone() const = 0;

      /*!
       * @brief If current MPI requests have completed, proceed to the
       * next communication wait (or to completion if no more
       * communication is needed).
       */
      virtual bool
      proceedToNextWait() = 0;

      /*!
       * @brief Complete entire operation, including waiting for
       * messages perform follow-up operations and waiting for
       * follow-ups to complete.
       *
       * After this method, isDone() should return true.
       *
       * This method should assume that all communications can finish.
       * Otherwise communications may hang.
       */
      virtual void
      completeCurrentOperation() = 0;

      /*!
       * @brief Set a user-defined Handler.
       *
       * A Handler is just some user-defined data.
       *
       * When the stage returns completed Members, the Handler makes
       * it easier to determine what was waiting for the Member to
       * complete.  If you do not need to find the handler this way,
       * you do not need to set it.  Typically, Handlers are only
       * needed if a stage is simultaneously coordinating multiple
       * Members per processor.
       *
       * By design, a non-const Handler is returned, even by a const
       * Member.  This is because the Handler is expected to follow
       * the communication operation with something non-trivial and
       * possibly state-changing.
       */
      void
      setHandler(
         Handler* handler);

      /*
       * @brief Regurgitate the handler from setHandler().
       */
      Handler *
      getHandler() const;

protected:
      /*!
       * @brief Associate the member with a stage.
       *
       * Specify the stage to use and the number of SAMRAI_MPI::Request
       * objects needed from the stage.
       *
       * @param nreq Number of SAMRAI_MPI::Requests needed by the member.
       *
       * @param stage Register with this AsyncCommStage.
       *
       * @param handler See getHandler()
       */
      void
      attachStage(
         const size_t nreq,
         AsyncCommStage* stage);

      /*!
       * @brief Disassociate the member from its stage (undo attachStage()).
       */
      void
      detachStage();

      /*!
       * Return the MPI requests provided by the stage for the Member.
       *
       * To work on the stage, you must use these requests in your
       * non-blocking MPI calls.  The requests are a part of an array
       * of requests for all Members on the stage.  Use only
       * getNumberOfRequests() requests.  Terrible bugs will strike if
       * you modify more than what was allocated for the Member.
       *
       * @b Important: Do not save the pointer returned by this
       * method.  The stage's dynamic memory actions may render old
       * pointers invalid.
       */
      SAMRAI_MPI::Request *
      getRequestPointer() const;

      /*!
       * @brief Return MPI statuses for this Member of the stage.
       *
       * To work on the stage, you must use these statuses in your MPI
       * calls.  The status attributes are set by the stage only for
       * requests completed by the stage.  The statuses are a part of
       * an array of statuses for all Members on the stage.  Use only
       * getNumberOfRequests() statuses.  Terrible bugs will strike if
       * you modify more than what was allocated for the Member.
       *
       * @b Important: Do not save the pointer returned by this
       * method.  The stage's dynamic memory actions may render old
       * pointers invalid.
       */
      SAMRAI_MPI::Status *
      getStatusPointer() const;

private:
      /*!
       * @brief Member is an integral part of the stage code and the
       * stage will set the Member's internal data.
       */
      friend class AsyncCommStage;

      /*!
       * @brief The stage this Member belongs to.
       *
       * This stage provides SAMRAI_MPI::Request and SAMRAI_MPI::Status
       * objects for the Member.
       */
      AsyncCommStage* d_stage;

      /*!
       * @brief Number of requests reserved on the stage, valid only
       * when d_stage is set.
       *
       * This parameter is always the same as
       * d_stage->numberOfRequests(d_index_on_stage), and exists only
       * for convenience.  Set by the stage when Member is staged or
       * destaged.
       */
      size_t d_nreq;

      /*!
       * @brief Member's index within the stage, valid only when
       * d_stage is set.
       *
       * Set by the stage when Member is staged or destaged.
       */
      size_t d_index_on_stage;

      /*!
       * @brief Pointer to the Member's Handler.
       *
       * @see getHandler()
       */
      Handler* d_handler;
   };

   /*!
    * @brief Container of pointers to Members.
    *
    * This container is used in some interfaces
    * and is instantiated in the SAMRAI library.
    */
   typedef std::vector<Member *> MemberVec;

   /*!
    * @brief Construct a stage that may begin allocating and
    * managing Members.
    */
   AsyncCommStage();

   /*!
    * @brief Deallocate Members remaining in the stage and all
    * internal data used to manage Members.
    */
   virtual ~AsyncCommStage(
      void);

   /*!
    * @brief Advance to completion one Member (any Member) that is
    * currently waiting for communication to complete.
    *
    * This method uses MPI_Waitany, which may be prone to starvation.
    * When a process is potentially receiving multiple messages from
    * another processor, it is better to use advanceSome(), which uses
    * MPI_Waitsome, which avoids starvation.
    *
    * @return completed Member, or NULL if no Member in the stage had
    * incomplete communication, or there are no Members on the stage.
    */
   Member *
   advanceAny();

   /*!
    * @brief Alternate interface for advanceAny to appear similar to other
    * advancing interfaces.
    *
    * @param completed Array space to put addresses of Member's
    *                  associated with the completed communications.
    *                  The array has length 0 or 1 on return.
    *
    * @return number of Members completed or 0 if none have pending
    * communication.  The strategy pointers corresponding to the
    * completed Members are set in the @c completed array.
    */
   size_t
   advanceAny(
      MemberVec& completed);

   /*!
    * @brief Advance to completion one or more Members (any Members)
    * that are currently waiting for communication to complete.
    *
    * @param completed Space to put addresses of Member's
    *                  associated with the completed communications.
    *
    * @return number of Members completed or 0 if none have pending
    * communication.
    */
   size_t
   advanceSome(
      MemberVec& completed);

   /*!
    * @brief Advance all pending communications to operations
    *
    * This method just calls advanceSome() repeatedly until all
    * operations complete.
    *
    * @param completed Array space to put addresses of Member's
    *                  associated with the completed communications.
    *
    * @return number of Members completed or 0 if none have pending
    * communication.
    */
   size_t
   advanceAll(
      MemberVec& completed);

   /*!
    * @brief Set optional timer for timing communication wait.
    *
    * The timer is used only for the MPI communication wait.  Timing
    * does not include time the stage Members use to process the
    * messages (the time spent in Member::proceedToNextWait()).
    *
    * If not set, or NULL is given, none will be used.
    */
   void
   setCommunicationWaitTimer(
      Pointer<Timer> communication_timer);

   /*!
    * @brief Get the number of Members on this stage.
    */
   size_t
   numberOfMembers() const;

   /*!
    * @brief Return whether the stage has any pending communication
    * requests.
    */
   bool
   hasPendingRequests() const;

   /*!
    * @brief Return the number of Members that have pending
    * communication.
    */
   size_t
   numberOfPendingMembers() const;

   /*!
    * @brief Return the number of pending SAMRAI_MPI::Request
    * objects on the stage.
    */
   size_t
   numberOfPendingRequests() const;

private:
   /*!
    * @brief Member is a friend so it can access private look-up and
    * stage/destage methods.  This avoids making those private methods
    * public.  This friendship is safe because Member is an integral
    * part of the stage code.
    */
   friend class Member;

   /*!
    * @brief Trivial typedef used by SAMRAI template scripts to
    * generate code to instantiate MemberVec (not
    * used anywhere else).
    */
   typedef Member * MemberPtr;

   /*!
    * @brief Set up a Member to work this stage, initializing mutual
    * references between the stage and the member.
    *
    * @param nreq Number of requests needed on the stage.
    *
    * @param handle Optional pointer back to a Member object associated
    *               with the Member.  See class documentation.
    */
   void
   privateStageMember(
      Member* Member,
      size_t nreq);

   /*!
    * @brief Remove a member from the stage, clearing mutual
    * references between the stage and the member.
    */
   void
   privateDestageMember(
      Member* Member);

   /*!
    * @brief Get the number of requests for the given Member index.
    */
   size_t
   numberOfRequests(
      size_t index_on_stage) const;

   /*!
    * @brief Assert internal data consistency.
    */
   void
   assertDataConsistency() const;

   /*!
    * @brief Lookup and return the request pointer from the stage for
    * the given Member.
    *
    * The given Member MUST have been allocated by the stage.
    * The number of requests that the Member may use is the
    * same as the number originally requested.
    *
    * The pointer is NOT guaranteed to be the same for the life
    * of the Member, as a stage may rearange the array of
    * SAMRAI_MPI::Request objects.  However, the pointer is valid
    * until the next call to privateStageMember().
    *
    * This is private because only friend class Member should
    * use it.
    */
   SAMRAI_MPI::Request *
   lookupRequestPointer(
      const size_t index_on_stage) const;

   /*!
    * @brief Look up and return the status pointer from the stage
    * for the given Member.  (Works similarly to lookupRequestPointer().)
    */
   SAMRAI_MPI::Status *
   lookupStatusPointer(
      const size_t index_on_stage) const;

   /*!
    * @brief Members managed on this stage.
    *
    * Includes destaged Members that are still occupying space on the
    * stage because they are not at the end of the vector and cannot
    * be removed from the vector.
    */
   MemberVec d_members;

   /*!
    * @brief Number of members.
    *
    * Not necessarily the same as d_members.size(), because d_members
    * may have unused slots left behind by destaged members.
    */
   size_t d_member_count;

   /*!
    * @brief SAMRAI_MPI::Request objects used by the Members.
    *
    * This is mutable because the const method getRequestPointer()
    * needs to return a non-const SAMRAI_MPI::Request pointer.  The
    * pointer must be non-const for use in MPI calls.
    * getRequestPointer() should be const because no Member should
    * require a non-const stage just to get the request allocated for
    * it.
    */
   mutable std::vector<SAMRAI_MPI::Request> d_req;

   /*!
    * @brief SAMRAI_MPI::Status objects corresponding to requests on
    * the stage.
    */
   mutable std::vector<SAMRAI_MPI::Status> d_stat;

   //!@brief Map from request index to Member index.
   std::vector<size_t> d_req_to_member;

   /*!
    * @brief Map from Member index to (the Member's first) request index.
    *
    * Provides the index where Member i's requests starts
    * (d_member_to_req[i]) and the number of request it has
    * (d_member_to_req[i+1]-d_member_to_req[i]).  This vector is
    * always one longer than d_members.  The extra item,
    * d_member_to_req[d_members.size()], is the total number of
    * requests that the stage has allocated.
    */
   std::vector<size_t> d_member_to_req;

   /*!
    * @brief Timer for communicaiton wait, set by
    * setCommunicationWaitTimer().
    */
   Pointer<Timer> d_communication_timer;

};

}
}

#endif  // included_tbox_AsyncCommStage
