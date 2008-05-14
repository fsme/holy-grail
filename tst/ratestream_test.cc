

#include <iostream>
#include <stdexcept>

#include <ratestream.hpp>

using namespace std;

int main ()
{
try {

	fx::ifrate quotes ("/var/log/robotrade.log");
	if ( !quotes.is_open ())
		throw runtime_error ("log do not open");

	std::string line;

	while ( std::getline ( quotes, line))
	{
		cout << line << endl;
	}

} catch (std::exception& e) {
		cout << e.what() << endl;
	} 

}
