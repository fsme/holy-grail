

#include <iostream>
#include <stdexcept>

#include <average.hpp>

using namespace std;

int main ()
{
try {

	fi::fo<float>	float_fifo (5);
	fi::fo<int32_t> int_fifo (5);

	ave::delta_FIFO_sum	summ_unit (3);

	float f = 1.5;

	for (int i = 0; i < 5; ++i)
	{
		float_fifo.push_back (f++);
		int_fifo.push_back (i);
		summ_unit.recomp ( float_fifo.back(), int_fifo );
	}

	float_fifo.print ("FLOAT="); logs << "" << endl;
	int_fifo.print ("INT="); logs << "" << endl;
	summ_unit.print (); logs << "" << endl;

} catch (std::exception& e) {
		cout << e.what() << endl;
	} 

}
