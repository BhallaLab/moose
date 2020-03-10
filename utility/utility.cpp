/***
 *    Description:  Utility functions.
 *
 *        Created:  2020-02-23

 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *        License:  MIT License
 */

#include "utility.h"

namespace moose {

    void splitIntervalInNParts(size_t max, size_t n, std::vector<std::pair<size_t, size_t>>& result)
    {
        size_t start = 0;
        size_t stop = 0;
        while(n > 0)
        {
            size_t f = (size_t) (max / n);
            stop += f;
            result.push_back({start, stop});
            start = stop;
            n -= 1;
            max -= f;
        }
        assert(max == 0);
    }

} // namespace moose
