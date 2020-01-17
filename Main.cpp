#include <iostream>
#include "Modifier.h"
#include <csignal>

int usage(char* argv0);

int main(int argc, char** argv) {
	if (argc != 3)
		return usage(argv[0]);

	static bool loop = true;
	signal(SIGINT, [](int)->void {loop = false; });

	Modifier modifier(argv[1], argv[2]);
	while (loop)
		modifier.modify();

	std::cout << "Terminated.\n";
	std::cin.get();
	return 0;
}

int usage(char* argv0) {
	std::cout << "Usage: " << argv0 << " <from_string> <to_string>" << std::endl;
	std::cin.get();
	return 0;
}
