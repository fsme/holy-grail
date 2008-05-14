
#include <sys/types.h>

#include <iostream>

using namespace std;

#include <average.hpp>

int main ()
{
try {
	fi::fo<delta_rate> Test (5);

	Test.push_back ( delta_rate (10, 1));
	Test.push_back ( delta_rate (11, 1));
	Test.push_back ( delta_rate (12, 1));
	Test.push_back ( delta_rate (13, 1));
	Test.push_back ( delta_rate (15, 2));
	Test.push_back ( delta_rate (16, 1));

cout << Test[0].delta
	 << "|"
	 << Test[1].delta
	 << "|"
	 << Test[2].delta
	 << "|"
	 << Test[3].delta
	 << "|"
	 << Test[4].delta
	 << endl;

	ave::delta Delta (3);

	Delta.rehash (Test);
	Delta.rehash (Test);
	Delta.rehash (Test);
	Delta.rehash (Test);
	Delta.rehash (Test);
	Delta.rehash (Test);

	cout << Delta.sum () << endl;

} catch (std::exception& e) {
		cout << e.what() << endl;
	} 

}
