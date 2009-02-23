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


//////////////////////
// CLASS DEFINITION //
//////////////////////

template <class T>
class Thread
{
    public:
        // Construction and destruction
        Thread(T& inputContainer);

        // Range calculation
        void split();

        // Iterators
        typename T::iterator begin;
        typename T::iterator end;

    private:

        // Internal data
        T* dataContainer;

        // Internal data (multithreaded)
        #ifdef WITH_OPENMP
        int dataSize;
        int dataThreads;
        #endif



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
    // Save size
    #ifdef WITH_OPENMP
    dataThreads = omp_get_num_threads();
    if (dataThreads > 1)
        dataSize = inputContainer.size();
    #endif

    // Save container
    dataContainer = &inputContainer;
}

//
// Range calculation
//

// Split the range without using OpenMP
#ifndef WITH_OPENMP
template <class T>
void Thread<T>::split()
{
    // Return the complete range
    begin = inputContainer.begin();
    end = inputContainer.end();
}
#endif

// Split the range using OpenMP
#ifdef WITH_OPENMP
template <class T>
void Thread<T>::split()
{
    // Single threaded work: return the complete range
    if (dataThreads == 1)
    {
        begin = dataContainer->begin();
        end = dataContainer->end();
        return;
    }

    // Multi threaded work, split the range
    else
    {
        // Get current thread
        int thread = omp_get_thread_num();

        // Starting point
        begin = dataContainer->begin();

        // Calculate remainder and quotient
		int remainder = dataSize % dataThreads;
		int quotient = dataSize / dataThreads;

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

		return;
    }

}
#endif

// Include guard
#endif
