/*
 * threading.h
 * Inkpad threading support.
 *
 * Original source by Neil Hunt, with following copyright:
 *   You are free to use this code as you see fit,
 *   but I bear no liability for the use of this code.
 * See http://blog.instedit.com/2008/05/openmp-utility-code.html
 * for more details.
 *
 * Copyright (c) 2008 Neil Hunt
 * Copyright (c) 2009 Tim Besard <tim.besard@gmail.com>
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Important note
 * ~~~~~~~~~~~~~~
 *
 * When using the thread library to process a container which gets altered
 * during the processing, two things have to be kept in mind:
 *     1) The construction of the Thread object has to be barried, in order
 *        to get evenly distributed slices originating from identical
 *        iterators;
 *     2) The Thread.begin iterator _mustn't_ be erased, as it equals to
 *        the Thread.end iterator of another thread and will probably cause
 *        invalid reads (memory contents can get altered though, the iterator
 *        should just be kept valid).
 */

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Include guard
#ifndef __THREADING
#define __THREADING

// Multithreading
#ifdef WITH_OPENMP
#include <omp.h>
#include <algorithm>
#endif



////////////
// MACROS //
////////////

#ifdef WITH_OPENMP
#define PARALLEL _Pragma("omp parallel")
#else
#define PARALLEL
#endif



//////////////////////
// CLASS DEFINITION //
//////////////////////

template <class T>
class Thread
{
    public:
        // Construction and destruction
        Thread(T& inputContainer);
        ~Thread();

        // Iterators
        typename T::iterator begin;
        typename T::iterator end;
        typename T::iterator nearend;

        // Debugging routines
        #ifndef NDEBUG
        int dbgThreadCount();
        int dbgThreadNumber();
        bool dbgMultithreaded();
        #endif

    private:
        // Range calculation
        void split_range(T& inputContainer);
};


////////////////////
// CLASS ROUTINES //
////////////////////


//
// Construction and destruction
//

template <class T>
Thread<T>::Thread(T& inputContainer)
{
    // Split the range
    split_range(inputContainer);
}

template <class T>
Thread<T>::~Thread()
{
}


//
// Range calculation
//

// Split the range without using OpenMP
#ifndef WITH_OPENMP
template <class T>
void Thread<T>::split_range(T& inputContainer)
{
    // Return the complete range
    begin = inputContainer.begin();
    end = inputContainer.end();
}
#endif

// Split the range using OpenMP
#ifdef WITH_OPENMP
template <class T>
void Thread<T>::split_range(T& inputContainer)
{
    // Calculate amount of threads
    int thread_count = omp_get_num_threads();

    // Single threaded work: return the complete range
    if (thread_count == 1)
    {
        begin = inputContainer.begin();
        end = inputContainer.end();
        return;
    }

    // Multi threaded work, split the range
    else
    {
        // Get current thread
        int thread = omp_get_thread_num();

        // Starting point
        begin = inputContainer.begin();

        // Fetch size
        int size;
        #pragma omp single copyprivate(size)
        {
            size = inputContainer.size();
        }

        // Calculate remainder and quotient
		int remainder = size % thread_count;
		int quotient = size / thread_count;

        // Math
		if (thread < remainder)
		{
			std::advance(begin, thread * (1 + quotient));
			end = begin;
			std::advance(end, quotient + 1);
		}
		else
        {
            std::advance(begin, remainder + thread * quotient);
            end = begin;
            std::advance(end, quotient);
        }
        nearend = end;
        nearend--;
		return;
    }

}
#endif


//
// Debugging routines
//

#ifndef NDEBUG

// Amount of threads
template <class T>
int Thread<T>::dbgThreadCount() {
    #ifdef WITH_OPENMP
    return omp_get_num_threads();
    #else
    return 1;
    #endif
}

// Number of current thread
template <class T>
int Thread<T>::dbgThreadNumber() {
    #ifdef WITH_OPENMP
    return omp_get_thread_num();
    #else
    return 1;
    #endif
}

// Check if multithreaded
template <class T>
bool Thread<T>::dbgMultithreaded() {
    #ifdef WITH_OPENMP
    return true;
    #else
    return false;
    #endif
}

#endif

// Include guard
#endif
