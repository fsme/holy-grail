//@(#) Static last receive rate for global use
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <rate_stats.h>

namespace stat {

fi::fo<float>	rates::BidFIFO;	///< FIFO of Bids
fi::fo<float>	rates::AskFIFO;	///< FIFO of Asks
fi::fo<int32_t>	rates::DltFIFO;	///< FIFO of delta

} //::stat
