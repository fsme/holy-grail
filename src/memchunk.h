//@(#) Memory allocation for receive data from cURL 
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _MEM_CHUNK_H
#define _MEM_CHUNK_H 1

#include <iron.h>

using namespace std;
using namespace cxx;
using namespace env;

namespace mem {

///\class chunk
///\brief Receive data from cURL to memory only

class chunk
{
typedef std::string::size_type	size_type;
private:
	static std::string _data; ///< Data of chunk
	
public:
///\brief Create
	chunk () { clear(); }

typedef size_t (*func) (void*, size_t, size_t, void*);

///\brief Get write function
///\return The function to write
func get_write_func () const { return (func) &write_func; }

///\brief Write data to memory
///\return Size of data
static size_t write_func (
	  void *ptr_ 	///\param ptr_ Pointer to new data
	, size_t size_	///\param size_ Size of new data
	, size_t nmemb_	///\param nmemb_ Size of block of the new
);

///\brief Get data as string
///\return Data
const std::string& str () const { return _data; }

///\brief Clear chunk
static void clear () { _data.clear(); }

}; //.class chunk

} //::mem

#endif //_MEM_CHUNK_H
