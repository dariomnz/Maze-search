#ifndef Debug_H
#define Debug_H

#include <iostream>

#define DEBUG_MSG(fmt) std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << __func__ << "(): " << fmt << std::endl;
#define DEBUG_ERROR(fmt) std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << __func__ << "() ERROR: " << fmt << std::endl;

#endif // Debug_H