/*
 * multithread.h
 * Inkpad multithreading support.
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
#ifndef __MULTITHREAD
#define __MULTITHREAD

// System headers
#include <omp.h>
#include <boost/range.hpp>


/////////////////
// SPLIT RANGE //
/////////////////

///////////////////////////////////////////////
//
// The returned sub range is such that if this function is called
// for each partition [0,partition_count), the entire "range"
// will be covered by all returned sub ranges, and distributed
// amongst the partitions in the most even size distribution possible.
//
// The size parameter must specify the size of the range.
// This overload, accepting a size, is preferable where
// range.size() may be expensive.
//
template<typename Range>
boost::iterator_range<typename Range::iterator> split_range(const Range& range, int partition_count, int partition, int size)
{
	typename Range::iterator begin = boost::begin(range);
	typename Range::iterator end = boost::end(range);

	if (partition_count > 1)
	{
		int remainder = size % partition_count;
		int quotient = size / partition_count;

		if (partition < remainder)
		{
			std::advance(begin, partition * (1 + quotient));
			end = begin;
			std::advance(end, quotient + 1);
		}
		else
		{
			std::advance(begin, remainder + partition * quotient);
			end = begin;
			std::advance(end, quotient);
		}
	}

	return boost::make_iterator_range(begin, end);
}

///////////////////////////////////////////////
//
// The returned sub range is such that if this function is called
// for each partition [0,partition_count), the entire "range"
// will be covered by all returned sub ranges, and distributed
// amongst the partitions in the most even size distribution possible.
//
// Use this overload where range.size() is not expensive
// (i.e. Range::iterator models random_access_iterator )
//
template<typename Range>
boost::iterator_range<typename Range::iterator> split_range(const Range& range, int partition_count, int partition)
{
	return split_range(range, partition_count, partition, range.size());
}

///////////////////////////////////////////////
//
// This function should be called within a #pragma omp parallel
// block, and returns a sub_range of the input range.
//
// The returned sub range is such that if this function is called
// by each thread in the parallel thread group, the entirety of "range"
// will be covered by all threads, and distributed amongst the threads
// in the most even size distribution possible.
//
// The size parameter must specify the size of the range.
// This overload, accepting a size, is preferable where
// range.size() may be expensive.
//
template<typename Range>
boost::iterator_range<typename Range::iterator> split_range_openmp(const Range& range, int size)
{
	int thread_count = omp_get_num_threads();
	int thread = omp_get_thread_num();

	return split_range(range, thread_count, thread, size);
}

///////////////////////////////////////////////
//
// This function should be called within a #pragma omp parallel
// block, and returns a sub_range of the input range.
//
// The returned sub range is such that if this function is called
// by each thread in the parallel thread group, the entirety of "range"
// will be covered by all threads, and distributed amongst the threads
// in the most even size() distribution possible.
//
// Use this overload where range.size() is not expensive
// (i.e. Range::iterator models random_access_iterator )
//
template<typename Range>
boost::iterator_range<typename Range::iterator> split_range_openmp(const Range& range)
{
	return split_range_openmp(range, range.size());
}

// Include guard
#endif
