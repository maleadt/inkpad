
#include "../../ustd.h"
#include <list>
#include <vector>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[])
{
	//
	// Initialisation
	//

	// Amount of iteration
	if (argc != 2)
		exit(0);
	const int AMOUNT = atoi(argv[1]);

	// Time values
	clock_t start, end;
	double time;


	//
	// Vector - verification
	//

	// Message
	std::cout << "Vector - verification" << std::endl;

	// Add some numbers
	std::vector<int> vector_correct(10);
	ustd::vector<int> vector_verify(10);
	srand(std::time(0));
	for (int i = 0; i < 5; i++)
	{
		int number = rand()%100;
		vector_correct[i] = number;
		vector_verify[i] = number;
	}
	vector_correct.reserve(vector_correct.size()+5);
	vector_verify.reserve(vector_verify.size()+5);
	for (int i = 0; i < 10; i++)
	{
		int number = rand()%100;
		vector_correct.push_back(number);
		vector_verify.push_back(number);
	}

	// Print all numbers
	std::cout << "\t- Vector contains:\t\t";
	for (unsigned int i = 0; i < vector_verify.size(); i++)
		std::cout << vector_verify[i] << " ";
	std::cout << std::endl << "\t- Vector should contain:\t";
	for (unsigned int i = 0; i < vector_correct.size(); i++)
		std::cout << vector_correct[i] << " ";
	std::cout << std::endl;



	//
	// Vector test
	//

	// Message
	std::cout << "* Vector - speed test" << std::endl;

	// Vector
	srand(0);
	start = clock();
	std::vector<int> test1Vector;
	for (int i = 1; i <= 2*AMOUNT; i++)
		test1Vector.push_back(i);
	for (unsigned int i = 0; i < test1Vector.size(); i++)
		test1Vector[rand()%(2*AMOUNT)] = i;
		//test1Vector[i] = test1Vector[i-1] + test1Vector[i+2] - test1Vector[i-2] - test1Vector[i+1];
	for (int i = 1; i <= 2*AMOUNT; i++)
	{
		test1Vector.pop_back();
	}
	end = clock();
	time = (double)(end - start)/CLOCKS_PER_SEC;
	std::cout << "\tstd:\t" << time << " seconds." << std::endl;

	// Array
	srand(0);
	start = clock();
	ustd::vector<int> test1Array;
	for (int i = 1; i <= 2*AMOUNT; i++)
		test1Array.push_back(i);
	for (unsigned int i = 2; i < test1Array.size()-2; i++)
		test1Array[rand()%(2*AMOUNT)] = i;
		//test1Array[i] = test1Array[i-1] + test1Array[i+2] - test1Array[i-2] - test1Array[i+1];
	for (int i = 1; i <= 2*AMOUNT; i++)
	{
		test1Array.pop_back();
	}
	end = clock();
	time = (double)(end - start)/CLOCKS_PER_SEC;
	std::cout << "\tustd:\t" << time << " seconds." << std::endl;



	//
	// List - verification
	//

	// Message
	std::cout << "* List - verification" << std::endl;

	// Add some numbers
	ustd::list<int> list_verify;
	std::list<int> list_verify_correct;
	srand(std::time(0));
	for (int i = 0; i < 10; i++)
	{
		int number = rand()%100;
		int number2 = rand()%100;
		list_verify.push_back(number);
		list_verify_correct.push_back(number);
		list_verify.push_front(number2);
		list_verify_correct.push_front(number2);
	}
	list_verify.pop_front();
	list_verify_correct.pop_front();

	// Print all numbers
	std::cout << "\t- List contains:\t";
	while (!list_verify.empty())
	{
		std::cout << list_verify.front() << " ";
		list_verify.pop_front();
	}
	std::cout << std::endl << "\t- List should contain:\t";
	std::list<int>::const_iterator it = list_verify_correct.begin();
	while (it != list_verify_correct.end())
	{
		std::cout << *(it++) << " ";
	}
	std::cout << std::endl;


	//
	// List - speed test
	//

	// Message
	std::cout << "* List - speed test" << std::endl;

	// STD
	start = clock();
	std::list<int> test2std;
	for (int i = 1; i <= AMOUNT; i++)
		test2std.push_back(i);
	for (int i = 1; i <= AMOUNT; i++)
		test2std.push_front(i);
	for (int i = 1; i <= AMOUNT; i++)
	{
		test2std.pop_front();
		test2std.pop_back();
	}
	end = clock();
	time = (double)(end - start)/CLOCKS_PER_SEC;
	std::cout << "\tstd:\t" << time << " seconds." << std::endl;

	// SSTD
	start = clock();
	ustd::list<int> test2ustd;
	for (int i = 1; i <= AMOUNT; i++)
		test2ustd.push_back(i);
	for (int i = 1; i <= AMOUNT; i++)
		test2ustd.push_front(i);
	for (int i = 1; i <= AMOUNT; i++)
	{
		test2ustd.pop_front();
		test2ustd.pop_back();
	}
	end = clock();
	time = (double)(end - start)/CLOCKS_PER_SEC;
	std::cout << "\tustd:\t" << time << " seconds." << std::endl;
}
