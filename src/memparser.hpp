//@(#) Parse of memory of cURL response
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _MEM_PARSER_HPP
#define _MEM_PARSER_HPP 1

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <stdexcept>

using namespace std;

namespace mem {

///\class parser
///\brief Parser of memory (for HTTP-response)

class parser
{
private:
	xmlDoc *doc;
	std::string _data;
	
public:
	xmlNode* root_node;

///\brief Create
parser () : doc (0) {}

///\brief Create with data
parser (
	  const	char* data_ ///\param data_ Pointer to XML data
	, const ssize_t size_ ///\param size_ Size of XML data
)
	: doc (0)
{
	parse ( data_, size_ );
}

///\brief Distroy
~parser () { clear(); }

///\brief Parse memory
///\return Root node
xmlNode* parse (
	  const	char* data_///\param data_ Pointer to XML data
	, const ssize_t size_///\param size_ Size of XML data
) {
	_data.assign (data_, size_);
	free_doc ();
	doc = ::xmlReadMemory ( _data.data(), _data.size(), 0, 0, 0);
	if (0 >= doc)
		throw runtime_error ("could not parse XML");

	root_node = ::xmlDocGetRootElement (doc);

	if (0 >= root_node)
		throw runtime_error ("could not get root node");

	return root_node;
}

///\brief Clean up
const std::string& str () const { return _data; }

///\brief Clean up
void free_doc ()
{
	if (doc) { ::xmlFreeDoc (doc); doc = 0; }
}

///\brief Clean up
void clear ()
{
	free_doc ();
	::xmlCleanupParser (); 
}

}; //.class parser

} //::mem

#endif //_MEM_PARSER_HPP
