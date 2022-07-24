#ifdef _WIN64
#include <iostream>
int main() {

	return 0;
}
#else
#include <iostream>
int main() {
	std::cout << "Error : Please use x64 platform." << std::endl;
	return 0;
}
#endif
