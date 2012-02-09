#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          // our last best hope for uint16_t
#include <sched.h>
//#include <atomic.h>

typedef int int32;
typedef long LONG;
typedef int32 Atomic32;

#if 0
extern "C" {
// We use windows intrinsics when we can (they seem to be supported
// well on MSVC 8.0 and above).  Unfortunately, in some
// environments, <windows.h> and <intrin.h> have conflicting
// declarations of some other intrinsics, breaking compilation:
//   http://connect.microsoft.com/VisualStudio/feedback/details/262047
// Therefore, we simply declare the relevant intrinsics ourself.

// MinGW has a bug in the header files where it doesn't indicate the
// first argument is volatile -- they're not up to date.  See
//   http://readlist.com/lists/lists.sourceforge.net/mingw-users/0/3861.html
// We have to const_cast away the volatile to avoid compiler warnings.
// TODO(csilvers): remove this once MinGW has updated MinGW/include/winbase.h
inline LONG FastInterlockedCompareExchange(volatile LONG* ptr,
                                           LONG newval, LONG oldval) {
  return ::InterlockedCompareExchange(ptr, newval, oldval);
}
inline LONG FastInterlockedExchange(volatile LONG* ptr, LONG newval) {
  return ::InterlockedExchange(ptr, newval);
}
inline LONG FastInterlockedExchangeAdd(volatile LONG* ptr, LONG increment) {
  return ::InterlockedExchangeAdd(ptr, increment);
}

inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
                                         Atomic32 old_value,
                                         Atomic32 new_value) {
  LONG result = FastInterlockedCompareExchange(
      reinterpret_cast<volatile LONG*>(ptr),
      static_cast<LONG>(new_value),
      static_cast<LONG>(old_value));
  return static_cast<Atomic32>(result);
}

inline Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr,
                                         Atomic32 new_value) {
  LONG result = FastInterlockedExchange(
      reinterpret_cast<volatile LONG*>(ptr),
      static_cast<LONG>(new_value));
  return static_cast<Atomic32>(result);
}

inline Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr,
                                        Atomic32 increment) {
  return FastInterlockedExchangeAdd(
      reinterpret_cast<volatile LONG*>(ptr),
      static_cast<LONG>(increment)) + increment;
}

inline Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr,
                                          Atomic32 increment) {
  return Barrier_AtomicIncrement(ptr, increment);
}

inline void MemoryBarrier() {
  ::MemoryBarrier();
}

inline Atomic32 Acquire_CompareAndSwap(volatile Atomic32* ptr,
                                       Atomic32 old_value,
                                       Atomic32 new_value) {
  return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
}

inline Atomic32 Release_CompareAndSwap(volatile Atomic32* ptr,
                                       Atomic32 old_value,
                                       Atomic32 new_value) {
  return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
}

inline void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value) {
  *ptr = value;
}

inline void Acquire_Store(volatile Atomic32* ptr, Atomic32 value) {
  NoBarrier_AtomicExchange(ptr, value);
              // acts as a barrier in this implementation
}

inline void Release_Store(volatile Atomic32* ptr, Atomic32 value) {
  *ptr = value; // works w/o barrier for current Intel chips as of June 2005
  // See comments in Atomic64 version of Release_Store() below.
}

inline Atomic32 NoBarrier_Load(volatile const Atomic32* ptr) {
  return *ptr;
}

inline Atomic32 Acquire_Load(volatile const Atomic32* ptr) {
  Atomic32 value = *ptr;
  return value;
}

inline Atomic32 Release_Load(volatile const Atomic32* ptr) {
  MemoryBarrier();
  return *ptr;
}
#endif

class SpinLock {
 public:
  SpinLock() : lockword_(kSpinLockFree) { }

  // Acquire this SpinLock.
  // TODO(csilvers): uncomment the annotation when we figure out how to
  //                 support this macro with 0 args (see thread_annotations.h)
  inline void Lock() /*EXCLUSIVE_LOCK_FUNCTION()*/ {
    while ( !TryLock()) {
	    sched_yield();
	}
  }

  // Try to acquire this SpinLock without blocking and return true if the
  // acquisition was successful.  If the lock was not acquired, false is
  // returned.  If this SpinLock is free at the time of the call, TryLock
  // will return true with high probability.
  inline bool TryLock()  {
    bool res =
        (__sync_lock_test_and_set(&lockword_, kSpinLockHeld) == kSpinLockFree);
    if (res) {
      //ANNOTATE_RWLOCK_ACQUIRED(this, 1);
    }
    return res;
  }

  // Release this SpinLock, which must be held by the calling thread.
  // TODO(csilvers): uncomment the annotation when we figure out how to
  //                 support this macro with 0 args (see thread_annotations.h)
  inline void Unlock() /*UNLOCK_FUNCTION()*/ {
    __sync_lock_test_and_set(&lockword_, kSpinLockFree);
  }

 private:
  enum { kSpinLockFree = 0 };
  enum { kSpinLockHeld = 1 };
  enum { kSpinLockSleeper = 2 };

  volatile Atomic32 lockword_;

  //DISALLOW_COPY_AND_ASSIGN(SpinLock);
};

// Corresponding locker object that arranges to acquire a spinlock for
// the duration of a C++ scope.
class SpinLockHolder {
 private:
  SpinLock* lock_;
 public:
  inline explicit SpinLockHolder(SpinLock* l) 
      : lock_(l) {
    l->Lock();
  }
  // TODO(csilvers): uncomment the annotation when we figure out how to
  //                 support this macro with 0 args (see thread_annotations.h)
  inline ~SpinLockHolder() /*UNLOCK_FUNCTION()*/ { lock_->Unlock(); }
};