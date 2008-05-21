//@(#) Store (FIFO) of statistics of rates (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <stat_fifo.h>

namespace stat {

fi::fo<float>	fifo::BidFIFO (144);	///< FIFO of Bids
fi::fo<float>	fifo::AskFIFO (144);	///< FIFO of Asks
fi::fo<int32_t>	fifo::DltFIFO (144);	///< FIFO of delta

} //::stat
