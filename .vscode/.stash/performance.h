
#pragma once

#include <chrono>
#include <map>
#include <iostream>
#include <functional>

constexpr char NO_CATEGORY[] { "Uncategorized" };

struct Measurement
{
	std::chrono::steady_clock::duration duration { };
	std::chrono::steady_clock::time_point start_point;
	size_t count { 0 };

	void start();
	void stop();

	long double percent(std::chrono::steady_clock::duration total);
	long double nanoseconds();
	std::string tostring(std::chrono::steady_clock::duration total);
};

class Timer
{
private:
	std::map<std::string, Measurement> values {};
	std::string current { NO_CATEGORY };
	Measurement overall { };
	bool is_open { false };
public:
	void start() { overall.start(); }
	void stop() { overall.stop(); }

	void open(std::string category);
	void close();
	
	template<class T>
	T call(std::string category, std::function<T()> func) {
		open(category);
		T value = func();
		close();
		return value;
	}
	void report();
};
