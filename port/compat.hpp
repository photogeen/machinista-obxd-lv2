#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <climits>
#include <cstring>
#include <string>
using String=std::string;
namespace juce { constexpr float float_Pi=3.14159265358979323846f; }
using juce::float_Pi;
template<class T> T jmin(T a,T b){return std::min(a,b);}
template<class T> T jlimit(T lo,T hi,T x){return std::clamp(x,lo,hi);}
inline int roundToInt(double x){return int(std::lrint(x));}
inline void zeromem(void* p,size_t n){std::memset(p,0,n);}
// Small independent pseudorandom utility for the engine's analog variation.
class Random {
 uint64_t state;
public:
 explicit Random(uint64_t seed=0x4f425844):state(seed?seed:1){}
 uint64_t nextInt64(){state^=state>>12;state^=state<<25;state^=state>>27;return state*2685821657736338717ULL;}
 float nextFloat(){return float(nextInt64()>>40)*(1.0f/16777216.0f);}
 static Random& getSystemRandom(){static thread_local Random r;return r;}
};
