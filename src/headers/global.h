#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <string>
#include <iostream>
#include <chrono>

template <typename T>
using s_ptr = std::shared_ptr<T>;
template <typename T>
using w_ptr = std::weak_ptr<T>;
using std::make_shared;

using std::string;
using std::cout;
using std::endl;

using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;

#endif // !GLOBAL_H