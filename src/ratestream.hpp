//@(#) Input stream of rates from quotes.log
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _FX_RATESTREAM_HPP
#define _FX_RATESTREAM_HPP 1

#include <fcntl.h>

#include <streambuf>
#include <istream>
#include <string>

using namespace std;

namespace fx {

static const streamsize ibuff_size = 8192;

///\class rateifstreambuf
///\brief Rate input from log-file stream buffer

template <typename C, typename T = std::char_traits<C> >
class rateifstreambuf
	: public std::basic_streambuf<C,T>
{
public:
typedef T		traits_type;
typedef typename T::int_type	int_type;
typedef typename T::pos_type	pos_type;

///\brief Create without args
rateifstreambuf ()
		: _is_open (false)
{}

///\brief Create with filename
rateifstreambuf (
	  const char* path_ ///\param path_ To input quotes.log file
)
	: _is_open (false)
{
	open (path_);
}

///\brief Destroy
virtual ~rateifstreambuf () { close (); }

///\brief Open this
void open (
	  const char* path_ ///\param path_ To input quotes.log file
) {
	if ( is_open()) close();

	quoteslog = ::open (path_, O_RDONLY | O_NONBLOCK );
	if (quoteslog < 0) return;

	this->setg (ibuff, ibuff, ibuff);
	_is_open = true ;
}

///\brief Close rateifstreambuf
void close ()
{
	::close (quoteslog);
	_is_open = false;
	this->setg (ibuff,ibuff,ibuff);
}

///\brief Is open
///\return True if it is open
bool is_open () const { return _is_open; }


protected:

///\brief Underflow
///\return First received char
virtual int_type underflow ()
{
	if ( this->gptr() < this->egptr() || fill_ibuf ())
		return traits_type::to_int_type ( *this->gptr ());
	//else
		this->setg (ibuff,ibuff,ibuff);
		return traits_type::eof();
}


///\brief Filling input buffer
///\return False if fail
bool
	fill_ibuf ()
{
	if ( !is_open ()) return false;

	ssize_t n;

	while ( (n = ::read ( quoteslog, ibuff, sizeof (ibuff) )) <= 0 )
	{
		if (n < 0) return false;
		else
			msleep (200);
	}
	this->setg (ibuff, ibuff, ibuff+n);
	return true;
}

private:
	rateifstreambuf<C,T> (const rateifstreambuf<C,T>&);
	rateifstreambuf<C,T>& operator=(const rateifstreambuf<C,T>&);

	C ibuff [ibuff_size];	///< Input buffer
	int quoteslog;			///< Log file descriptor
	bool _is_open;

///\brief Sleep at millisecond
int msleep (unsigned long millisec)
{
    struct timespec req={0};
    time_t sec=(int)(millisec/1000);
    millisec=millisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=millisec*1000000L;
    while ( ::nanosleep(&req,&req)==-1)
         continue;
    return 1;
}

}; //class rateifstreambuf



///\class rateif_ios 
///\brief Base IOS for fx::rateifstream

template <typename C, typename T = std::char_traits<C> >
class rateif_ios
	: virtual public std::basic_ios<C,T>
{
protected:

///\brief Constructor by default
rateif_ios ()
	: std::basic_ios<C,T>()
	, ifbuf()
{
	this->init (&ifbuf);
}

///\brief Open 
explicit rateif_ios (
	  const char* path_ ///\param path_ To input quotes.log
)
	: std::basic_ios<C,T> (0)
	, ifbuf (path_)
{
	this->init (&ifbuf);
}

///\brief Open 
explicit rateif_ios (
	  const std::string& path_ ///\param path_ To input quotes.log
)
	: std::basic_ios<C,T> (0)
	, ifbuf (path_.c_str ())
{
	this->init (&ifbuf);
}

///\brief Destructor
virtual ~rateif_ios () { this->close(); }


	rateifstreambuf<C,T> ifbuf; ///< Input stream from quotes.log buffer


public:

///\brief Open 
void open (
		const char* path_ ///\param path_ To input quotes.log
) {
	ifbuf.open (path_);
}

///\brief Open 
void open (
		const std::string& path_ ///\param path_ To input quotes.log
) {
	open (path_.c_str());
}

///\brief Close
void close () { ifbuf.close (); }

///\brief Is open
///\return True if is open

bool is_open () const { return ifbuf.is_open (); }

///\brief Get pointer to stream buffer
///\return Pointer to the buffer

rateifstreambuf<C,T>* rdbuf () { return &ifbuf; }

}; //.class rateif_ios


///\class rateifstream 
///\brief Rates input from quotes.log stream

template <typename C, typename T = std::char_traits<C> >
class rateifstream
	: public std::basic_istream<C,T>
	, public rateif_ios<C,T>
{
private:

typedef std::basic_istream<C,T> base_type;
typedef rateif_ios<C,T> base_ios;

	using base_ios::ifbuf;

public:

///\brief Constructor by default
rateifstream () : base_type(0), base_ios() {}

///\brief Open
explicit
rateifstream (
	  const char* path_ ///\param 
)
	: base_type(0)
	, base_ios (path_)
{}

///\brief Open
explicit
rateifstream (
	  const std::string& path_ ///\param 
)
	: base_type(0)
	, base_ios (path_)
{}

}; //.class rateifstream

typedef rateifstream<char>	ifrate;

} //::fx

#endif // _FX_RATESTREAM_HPP
