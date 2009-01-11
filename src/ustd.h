/*
 * ustd.cpp
 * Minimal replacment for some STD data structures.
 *
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
#ifndef __USTD
#define __USTD


// Headers
#include <iostream>
#include <cmath>


//
// Constants
//

// Initial vector size if not given
const int VECTOR_SIZE_DEFAULT = 25;



////////////
// VECTOR //
////////////

//
// Class defenition
//

namespace ustd
{
	template <typename T>
	class vector
	{

		public:
			// Construction and destruction
			vector();
			vector(int size);
			vector(int size, T value);
			~vector();

			// Vector configuration
			void reserve(int size);
			void resize(int size);

			// Element input
			void push_back(T data);
			void insert(T data, int index);

			// Element output
			T front() const;
			T back() const;
			T& operator[](int);

			// Element removal
			void pop_back();
			void erase(int index);
			void clear();

			// Vector information
			unsigned int size() const;
			unsigned int capacity() const;

		private:
			// Array containing data
			T* dataArray;
			int dataArraySize;
			int dataArraySizeVirtual;
	};
}



//
// Construction and destruction
//

// Constructor
template <typename T>
ustd::vector<T>::vector()
{
	// Save size
	dataArraySize = VECTOR_SIZE_DEFAULT;
	dataArraySizeVirtual = 0;

	// Create new array
	dataArray = new T[dataArraySize];
}
template <typename T>
ustd::vector<T>::vector(int inputSize)
{
	// Save size
	dataArraySize = inputSize;
	dataArraySizeVirtual = inputSize;

	// Create new array
	dataArray = new T[dataArraySize];
}
template <typename T>
ustd::vector<T>::vector(int inputSize, T inputValue)
{
	// Save size
	dataArraySize = inputSize;
	dataArraySizeVirtual = inputSize;

	// Create new array
	dataArray = new T[dataArraySize];

	// Save default value
	for (int i = 0; i < dataArraySize; i++)
		dataArray[i] = inputValue;
}

// Destructor
template <typename T>
ustd::vector<T>::~vector()
{
	// Delete data array
	delete[] dataArray;
}


//
// Vector configuration
//

// Reserve a given size
template <typename T>
void ustd::vector<T>::reserve(int inputSize)
{
	// Move the old array to a temporary place
	T* temp = dataArray;

	// Allocate a new array with requested size
	dataArraySize = inputSize;
	dataArray = new T[dataArraySize];

	// Check if we need to copy old items
	int element = dataArraySizeVirtual;
	if (element > inputSize)
		element = dataArraySize;
	for (int i = 0; i < element; i++)
		dataArray[i] = temp[i];

	// Delete the old array
	delete[] temp;
}

// Resize the vector
template <typename T>
void ustd::vector<T>::resize(int inputSize)
{
	dataArraySizeVirtual = inputSize;

	if (dataArraySizeVirtual > dataArraySize)
		reserve(dataArraySizeVirtual);
}


//
// Element input
//

// Put a new item at the end of the vector
template <typename T>
void ustd::vector<T>::push_back(T inputItem)
{
	// Increase the virtual size
	dataArraySizeVirtual++;

	// Check if we need to allocate more space
	if (dataArraySizeVirtual > dataArraySize)
		reserve(dataArraySize*2);

	// Save the item
	dataArray[dataArraySizeVirtual-1] = inputItem;
}

// Insert an item at a specific place
template <typename T>
void ustd::vector<T>::insert(T inputItem, int inputIndex)
{
	// Reserve place if needed
	dataArraySizeVirtual++;
	if (dataArraySizeVirtual > dataArraySize)
		reserve(dataArraySizeVirtual);

	// Shift elements
	for (int i = dataArraySizeVirtual-2; i >= inputIndex; i--)
		dataArray[i+1] = dataArray[i];

	// Save element
	dataArray[inputIndex] = inputItem;
}



//
// Element output
//


// Front access
template <typename T>
T ustd::vector<T>::front() const
{
	return dataArray[0];
}

// Back access
template <typename T>
T ustd::vector<T>::back() const
{
	return dataArray[dataArraySizeVirtual-1];
}

// Square-bracket operator
template <typename T>
T& ustd::vector<T>::operator[](int inputIndex)
{
	return dataArray[inputIndex];
}


//
// Element removal
//

// Remove last item
template <typename T>
void ustd::vector<T>::pop_back()
{
	dataArraySizeVirtual--;
}

// Remove a specific item
template <typename T>
void ustd::vector<T>::erase(int inputIndex)
{
	for (int i = inputIndex+1; i < dataArraySizeVirtual; i++)
		dataArray[i-1] = dataArray[i];
	dataArraySizeVirtual--;
}

// Clear the vector
template <typename T>
void ustd::vector<T>::clear()
{
	dataArraySizeVirtual = 0;
}





//
// Vector information
//

// The current size
template <typename T>
unsigned int ustd::vector<T>::size() const
{
	return dataArraySizeVirtual;
}

// The current capacity
template <typename T>
unsigned int ustd::vector<T>::capacity() const
{
	return dataArraySize;
}



//////////
// LIST //
//////////


//
// Class defenition
//

namespace ustd
{
	template <typename T>
	class list
	{
		private:
			// The node structure
			struct Node
			{
				T data;
				Node *next;
				Node *prev;
			};

		public:
			// Construction and destruction
			list();
			~list();

			// Iterator
			struct iterator
			{
				// Current location
				Node* ptr;

				// Constructor (based on given node)
				iterator (Node* p = 0) : ptr(p) {}

				// Assignment-operator
				T operator= (const iterator& other)
				{
					ptr = other.ptr;
				}

				// Dereference-operator
				T& operator* ()
				{
					return ptr->data;
				}

				// Member by pointer-operator
				T* operator-> ()
				{
					return &(ptr->data);
				}


				// Move the iterator
				iterator& operator++ ()
				{
					ptr = ptr->next;
					return *this;
				}

				// Change value
				iterator operator++ (int)
				{
					iterator tmp = *this;
					++*this;
					return tmp;
				}

				// Comparison
				bool operator== (const iterator& other) const
				{
					return ptr == other.ptr;
				}
				bool operator!= (const iterator& other) const
				{
					return ptr != other.ptr;
				}
			};

			// Static iterators
			iterator begin()
			{
				return iterator(nodeFront);
			}
			iterator end()
			{
				return iterator(nodeBack->next);
			}

			// Element input
			void push_front(T data);
			void push_back(T data);
			void insert(iterator, T data);

			// Element output
			T front() const;
			T back() const;

			// Element removal
			void pop_front();
			void pop_back();
			// Erase: http://www.google.com/codesearch/p?hl=nl#721ufgYZrlQ/libg++-2.8.0/libstdc++/stl/stl_list.h&q=lang:c%2B%2B%20list%20iterator%20erase ook in header?????
			iterator erase(iterator inputIterator)	// TODO: memory leak
			{
				// Iterator to next item
				iterator outputIterator;
				outputIterator.ptr = inputIterator.ptr->next;

				// Remove the data
				pop_specific(inputIterator.ptr);

				return outputIterator;
			}
			void clear();	// TODO: memory leak

			// List information
			bool empty() const;
			unsigned int size() const;

		private:
			// Front and back nodes
			Node *nodeFront;
			Node *nodeBack;

			// Element input
			void push_before(T data, Node *NodeB);
			void push_after(T data, Node *NodeA);

			// Element removal
			void pop_before(Node *NodeB);
			void pop_after(Node *NodeA);
			void pop_specific(Node *newNode);
	};
}


//
// Construction and destruction
//

// Constructor
template <typename T>
ustd::list<T>::list()
{
	nodeFront = NULL;
	nodeBack = NULL;
}

// Destructor
template <typename T>
ustd::list<T>::~list()
{
	Node* iterator = nodeFront;
	while (iterator != NULL)
	{
		// We need to backup the current pointer, to be able to delete it later on
		Node* ToDelete = iterator;
		iterator = iterator->next;
		delete (ToDelete);
	}
	nodeFront = NULL;
	nodeBack = NULL;
}


//
// Element input
//

//insert a Node after the last Node
template <typename T>
void ustd::list<T>::push_back (T data)
{
	if (nodeBack==NULL)
	{
		push_front(data);
	}
	else
	{
		push_after(data,nodeBack );
	}
}


//insert a Node before the nodeFront Node
template <typename T>
void ustd::list<T>::push_front (T data)
{
	// List is empty
	if (nodeFront == NULL)
	{
		Node *newNode;
		newNode = new Node;
		nodeFront = newNode;
		nodeBack = newNode;
		newNode->prev = NULL;
		newNode->next = NULL;
		newNode->data = data;
	}

	// List is not empty, insert before first element
	else
	{
		push_before(data, nodeFront);
	}
}

// Insert an element before a given iterator
template <typename T>
void ustd::list<T>::insert(iterator inputIterator, T inputData)
{
	// Insert the data
	push_before(inputData, inputIterator.ptr);
}

//insert a Node before NodeB
template <typename T>
void ustd::list<T>::push_before(T data, Node *NodeB)
{
	Node *newNode;
	newNode = new Node;
	newNode->prev = NodeB->prev;
	newNode->next = NodeB;
	newNode->data = data;

	if (NodeB->prev == NULL)
	{
		nodeFront=newNode;
	}
	NodeB->prev = newNode;
}

//insert a Node after  NodeB
template <typename T>
void ustd::list<T>::push_after(T data,Node *NodeB)
{
	Node *newNode;
	newNode=new Node();
	newNode->next= NodeB->next ;
	newNode->prev  =NodeB;
	newNode->data =data;

	if (NodeB->next == NULL)
	{
		nodeBack =newNode;
	}
	NodeB->next=newNode;
}

//
// Element output
//

// Access first item
template <typename T>
T ustd::list<T>::front() const
{
	return nodeFront->data;
}

// Access the last item
template <typename T>
T ustd::list<T>::back() const
{
	return nodeBack->data;
}


//
// Element removal
//

//remove the nodeFront Node
template <typename T>
void ustd::list<T>::pop_front()
{
	pop_specific(nodeFront);
}

//remove a nodeBack Node
template <typename T>
void ustd::list<T>::pop_back()
{
	pop_specific(nodeBack);
}

// Clear the list
template <typename T>
void ustd::list<T>::clear()
{
	nodeFront = NULL;
	nodeBack = NULL;
}


//remove before a Node
template <typename T>
void ustd::list<T>::pop_before(Node *NodeB)
{

	if (NodeB->prev==nodeFront)
	{
		delete nodeFront;
		nodeFront=NodeB;
		nodeFront->prev=NULL;
	}
	else
	{
		pop_specific(NodeB->prev);
	}
}

//remove after a Node
template <typename T>
void ustd::list<T>::pop_after(Node *NodeA)
{
	if (NodeA->next==nodeBack)
	{
		delete nodeBack;
		nodeBack=NodeA;
		nodeBack->next=NULL;
	}
	else
	{
		pop_specific(NodeA->next);
	}
}

//remove a perticular Node
template <typename T>
void ustd::list<T>::pop_specific(Node *NodeToRemove)
{
	if (NodeToRemove == nodeFront && NodeToRemove == nodeBack)
	{
		nodeFront = NULL;
		nodeBack = NULL;
	}
	else if (NodeToRemove == nodeFront)
	{
		nodeFront = nodeFront->next;
		nodeFront->prev=NULL;
	}
	else if (NodeToRemove == nodeBack)
	{
		nodeBack = nodeBack->prev;
		nodeBack->next = NULL	;
	}
	else
	{
		NodeToRemove->prev->next = NodeToRemove->next;
		NodeToRemove->next->prev = NodeToRemove->prev;
	}
	delete NodeToRemove;
}


//
// List information
//

// Check if list is empty
template <typename T>
bool ustd::list<T>::empty() const
{
	return (nodeFront == NULL);
}

// Check if list is empty
template <typename T>
unsigned int ustd::list<T>::size() const
{
	Node* tempNode = nodeFront;

	if (tempNode == 0)
		return 0;

	int outputSize = 1;
	while (tempNode->next != 0)
	{
		outputSize++;
		tempNode = tempNode->next;
	}

	return outputSize;
}

#endif
