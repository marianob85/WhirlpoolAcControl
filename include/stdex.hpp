#pragma once

#ifdef ARDUINO
template< typename T >
uint8_t tounderlying( T t )
{
	return static_cast< uint8_t >( t );
}
#else
#include <utility>
template< typename E >
constexpr typename std::underlying_type< E >::type tounderlying( E e ) noexcept
{
	return static_cast< typename std::underlying_type< E >::type >( e );
}
#endif