#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <chrono>
#include <ranges>
#include "Objects.hpp"

using std::vector;
using std::unique_ptr;

class Simulation final {
public:
	void addObject(Positioning::Object2D& object) {
		_objects.push_back(object);
	}

private:
	vector<Positioning::Object2D> _objects;
	Objects::Worm worm;
};

#pragma region experiments
struct Test {
	Test() {
		id = idGenerator;
		idGenerator++;
		std::cout << "constructor " << id << std::endl;
	}

	Test(const Test& test) {
		id = test.id;
		std::cout << "copy " << id << std::endl;
	}

	Test(Test&& test) noexcept {
		id = test.id;
		test.id = -1;
		std::cout << "move " << id << std::endl;
	}

	~Test() {
		std::cout << "destructor " << id << std::endl;
	}
	int id;
	static int idGenerator;
};

int Test::idGenerator = 0;

std::vector<Test> testVecGen() {
	std::vector<Test> test;
	for (int c = 0; c < 3; c++) {
		test.push_back(Test());
	}
	return test;
}
#pragma endregion

#include "CognitiveSystem.hpp"
/*!
* The main function that is actually like a life cycle.
*/
int main(int argc, char** argv) {
	std::vector<int> vec{ 1,2,3,4,5,6,7,8,9,10 };
	std::vector<int> vec2{ 0,1,2,3,4,5,6,7,8,9 };
	//std::vector<int> resvec;

	for (auto iter = vec.rbegin() + 1; iter != vec.rend(); ++iter) {
		std::cout << *iter << ' ';
	}
	


	/*vector<int> vec;
	Worm worm;
	Object2D* obj = &worm;
	
	cout << "worm is a type of " << typeid(worm).name() << endl;
	cout << "obj is a type of " << typeid(*obj).name() << endl;

	cout << "obj is a type of " << typeid(*obj).name() << endl;*/
	return 0;
}
