#ifndef PSEUDO_RANDOM_SEQUENCES
#define PSEUDO_RANDOM_SEQUENCES

#include <vector>
#include <bitset>
#include <boost/math/distributions/chi_squared.hpp>
#include <chrono>

#ifdef __linux__
#include <unistd.h>
#include <sys/times.h>
#elif _WIN32
#include <time.h>
#endif

#include "i_statistical_test.h"

namespace statistical_tests_space {

using std::vector;
using std::pair;
using std::string;
typedef size_t Word;
const uint32_t MAX_DIMENSION = sizeof(Word) * 8;
using AlphabetType = std::bitset<MAX_DIMENSION>;

#ifdef __linux__
    using std::chrono::time_point;
    typedef time_point<std::chrono::steady_clock> TimeType;
#elif _WIN32   //Windows
    typedef int TimeType;
#endif

// return in milliseconds
TimeType my_get_current_clock_time();

int getTimeDifferenceInMillis(TimeType const & from, TimeType const & to);



vector<string> getStatisticTestNames(string testKey, size_t sequenceSize);

void runTests(BoolIterator epsilonBegin,
    BoolIterator epsilonEnd,
    std::vector<double> & testResults,
    std::string const & testKey,
    const std::string &uniqueSequenceName);

int beaconRun(int argc, char * argv[]);

int generatorsTestConfigRun(int argc, char * argv[]);

}

#endif //PSEUDO_RANDOM_SEQUENCES
