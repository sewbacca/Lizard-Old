
#include <sstream>

#include "performance.h"

void Measurement::start()
{
	++count;
	start_point = std::chrono::steady_clock::now();
}

void Measurement::stop()
{
	duration += std::chrono::steady_clock::now() - start_point;
}

long double Measurement::percent(std::chrono::steady_clock::duration total)
{
	long double dur { duration.count() };
	long double tot { total.count() };
	return dur / tot * 100;
}

long double Measurement::nanoseconds()
{
	return duration.count() / count;
}

std::string Measurement::tostring(std::chrono::steady_clock::duration total)
{
	std::stringstream output { };
	output << percent(total) << "% " << nanoseconds() << " ns " << count << " hits (" << duration.count() << " total ns)";

	return output.str();
}

void Timer::open(std::string category)
{
	close();
	
	if(values.count(category) == 0)
		values[category] = Measurement();

	current = category;
	values[current].start();
	is_open = true;
}

void Timer::close()
{
	if(is_open)
		values[current].stop();
	is_open = false;
}
void Timer::report()
{
	std::cout << "Timer:\n";
	auto unaccounted = overall;

	for(auto category : values)
	{
		unaccounted.duration -= category.second.duration;
		std::cout << '\t' << category.first << ": " << category.second.tostring(overall.duration) << std::endl;
	}

	std::cout << "\tUnacounted: " << unaccounted.tostring(overall.duration) << std::endl;
	std::cout << "\tTotal: " << overall.tostring(overall.duration) << std::endl;
}