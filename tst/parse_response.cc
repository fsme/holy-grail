
#include <iostream>
#include <sstream>
#include <fstream>

#include <memparser.hpp>

using namespace std;

int
 main ()
{
try {
	stringstream sstr;
	{	ifstream file ("buy_response.xml");
		file >> noskipws >> sstr.rdbuf(); 
	}
	auto_ptr<mem::parser> resp =
			auto_ptr<mem::parser>(\
					 new mem::parser ( sstr.str().data(), sstr.str().size() )
								);

/// Success true
	cout << (const char*)resp.get()->root_node->children->next->children->content << endl;

/// Error Description
	cout << (const char*)resp.get()->root_node->children->next->next->next->name << endl;

/// Error Number
	cout << (const char*)resp.get()->root_node->children->next->next->next->next->next->children->content << endl;

/// Confirmatiom
	sstr.str(""); sstr.clear();
	sstr << (const char*)resp.get()->root_node->children->next->next->next->next->next->next->next->children->content << flush;

	string line;
	while ( getline ( sstr, line ) )
	{
		cout << line << endl;
	}

} catch (const exception& e) {
		cout << e.what() << endl;
	}

}

