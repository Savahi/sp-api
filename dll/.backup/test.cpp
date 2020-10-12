#include <string>
#include <iostream>

class C1 {
	bool created;

	public:
	C1() {
		created=false;
	}

	~C1() {
	}

	operator bool() const {
		return created;
	}
	
};

int main() 
{
	C1 c1;
	if( c1 == false ) {
		std::cout << "not created!" << std::endl;
	}
}