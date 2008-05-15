//@(#) Receive data from cURL (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <memchunk.h>
#include <curl/curl.h>

namespace mem {

std::string chunk::_data;

//
size_t chunk::write_func (
	  void *ptr_
	, size_t size_
	, size_t nmemb_
) {
	size_type real_size = size_ * nmemb_;
	_data.append ( (const char*)ptr_, real_size);
	return _data.size ();
}

} //::mem
