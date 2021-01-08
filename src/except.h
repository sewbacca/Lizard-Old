
#ifndef EXCEPT_H
#define EXCEPT_H

#include <string>
#include <stdexcept>

constexpr void throw_assert(bool assertion, const char* error)
{
	if (!assertion)
		throw std::runtime_error(error);
}

#endif	      // EXCEPT_H