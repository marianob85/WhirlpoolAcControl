#include "Whirlpool_YJ1B.hpp"

WhirlpoolYJ1B::WhirlpoolYJ1B( const WhirlpoolYJ1BData& data ) : m_data{ data } {}

WhirlpoolYJ1BData& WhirlpoolYJ1B::data()
{
	return m_data;
}

#ifdef WhirlpoolYJ1B_TRACE
void WhirlpoolYJ1B::print() {}
#endif