//
// Lock.h
//
// Part of the FTC Team 417 Software Starter Kit: www.ftc417.org/ssk
//
// Definition of a lock mechanism that allows us to safely take turns accessing
// data from disparate tasks.

//--------------------------------------------------------------------------------------
// Variations on hogCPU/releaseCPU that work even when calls are nested
//--------------------------------------------------------------------------------------

int _cHogCPU = 0;

#define hogCpuNestable()        \
    {                           \
    hogCPU();                   \
    _cHogCPU += 1;              \
    }

#define releaseCpuNestable()    \
    {                           \
    _cHogCPU -= 1;              \
    if (0 == _cHogCPU)          \
        releaseCPU();           \
    }

//---------------------------------------------------------------------------------------
// LOCK definition and initialization
//---------------------------------------------------------------------------------------

typedef struct
    {
    int     owningTask;     // value of nCurrentTask for task which owns the lock, or -1 if no owner
    int     cRecursion;     // how many times owning task owns
    string  displayName;    // name of the lock for use in tracing messages
    } LOCK;

// Call InitializeLock for each of the locks you use in your program. Usually this is
// done in the initialization logic in task main() which precedes the call to waitForStart().
#define InitializeLock(lock,name)                 \
    {                                             \
    lock.owningTask = -1;                         \
    lock.cRecursion = 0;                          \
    lock.displayName = name;                      \
    }

//---------------------------------------------------------------------------------------
// LOCK utility functions
//---------------------------------------------------------------------------------------

#define SleepWaitingForLock()   EndTimeSlice()

#ifndef hogCpuNestable
#define hogCpuNestable()        hogCPU()
#endif

#ifndef hogCpuNestable
#define hogCpuNestable()        releaseCPU()
#endif

//---------------------------------------------------------------------------------------
// Core LOCK functions
//---------------------------------------------------------------------------------------

// Acquire ownership of this lock for the current task
#define LockExclusive(lock)                                                                 \
    {                                                                                       \
    hogCpuNestable();                                                                       \
    if (nCurrentTask == lock.owningTask)                                                    \
        {                                                                                   \
        /* this task already owns the lock */                                               \
        lock.cRecursion += 1; /* atomic */                                                  \
        }                                                                                   \
    else                                                                                    \
        {                                                                                   \
        /* does some other task own the lock? */                                            \
        if (-1 != lock.owningTask)                                                          \
            {                                                                               \
            /* yes, some other task owns. this task must wait until it releases it */       \
	        do                                                                              \
	            {                                                                           \
	            releaseCpuNestable();                                                       \
	            SleepWaitingForLock();                                                      \
	            hogCpuNestable();                                                           \
	            }                                                                           \
	        while (-1 != lock.owningTask);                                                  \
	        }                                                                               \
        /* by here, no task owns the lock. so we can just take it */                        \
        lock.owningTask = nCurrentTask;                                                     \
        lock.cRecursion = 1;                                                                \
        }                                                                                   \
    releaseCpuNestable();                                                                   \
    }

// Release the current task's ownership of this lock
#define ReleaseLock(lock)                                                                   \
    {                                                                                       \
    lock.cRecursion -= 1; /* atomic */                                                      \
    if (0 == lock.cRecursion)                                                               \
        {                                                                                   \
        lock.owningTask = -1; /* atomic */                                                  \
        }                                                                                   \
    }

// Style: use the following as a debugging aids only.
#define IsLockHeld(lock)       (lock.owningTask != -1)
#define CheckLockHeld(lock)    { if (!IsLockHeld(lock)) { writeDebugStreamLine("lock not held:%s", lock.displayName); } }

//---------------------------------------------------------------------------------------
// Locks used in the Three Task Architecture
//---------------------------------------------------------------------------------------

LOCK    lockBlackboard;           // must own to read or write the blackboard (and more: to be elaborated)
LOCK    lockDaisy;
// NOTE: If you are going to grab BOTH lockDaisy and lockBlackboard, you MUST
//       acquire lockBlackboard FIRST; otherwise, deadlock may occur between tasks.

//---------------------------------------------------------------------------------------
// Optional space-saving acquire/release functions for defined locks
//---------------------------------------------------------------------------------------

void LockBlackboard()
    {
    LockExclusive(lockBlackboard);
    }
void ReleaseBlackboard()
    {
    ReleaseLock(lockBlackboard);
    }

//--------------------------------------------------------------------------

void LockDaisy()
    {
    LockExclusive(lockDaisy);
    }
void ReleaseDaisy()
    {
    ReleaseLock(lockDaisy);
    }
