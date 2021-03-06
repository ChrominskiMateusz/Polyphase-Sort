#include "Logic.h"

int main (int argc, char* argv[])
{
	Logic<record> *tmp;

	if (argc == 1)
	{
		std::string fn;
		std::cout << "enter file name: ";
		std::cin >> fn;
		tmp = new Logic<record> (fn);

	}
	else if (argc == 2)
		tmp = new Logic<record> (argv[1]);
	else if (argc == 4)
		tmp = new Logic<record> (argv[1], atoi (argv[2]), atoi (argv[3]));
	else
		return 0;


	std::cout << "Print lines after every step?\n";
	std::cin >> tmp->doPrint;
	tmp->sort ();

	std::cout << "Operations : " << tmp->physical->operations << std::endl;
	std::cout << "F Count : " << tmp->fCount << std::endl;

	delete tmp;
	return 0;
}