/////////////
// GENERAL //
/////////////

// Include guard
#ifndef __ARRAY
#define __ARRAY

// Headers
#include <iostream>
#include <cmath>

// Constants
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

		public:
			// Construction and destruction
			list();
			~list();

			// Element input
			void push_front(T data);
			void push_back(T data);
			void insert(T data, int index);

			// Element output
			T front() const;
			T back() const;

			// Element removal
			void pop_front();
			void pop_back();
			void erase(int index);

			// List information
			bool empty();

		private:
			// The node structure
			struct Node
			{
				T data;
				Node *next;
				Node *prev;
			};

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

// Insert an element before a given index
template <typename T>
void ustd::list<T>::insert(T inputData, int inputIndex)
{
	// Go to the node
	Node* tempNode = nodeFront;
	for (int i = 0; i < inputIndex; i++)
		tempNode = tempNode->next;

	// Insert the data
	push_before(inputData, tempNode);
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

// Remove a given element
template <typename T>
void ustd::list<T>::erase(int inputIndex)
{
	// Go to the node
	Node* tempNode = nodeFront;
	for (int i = 0; i < inputIndex; i++)
		tempNode = tempNode->next;

	// Remove the data
	pop_specific(tempNode);
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
bool ustd::list<T>::empty()
{
	return (nodeFront == NULL);
}

#endif
