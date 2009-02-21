/*
 * multithread.cpp
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

// Headers
#include "multithread.h"
