#include "IdGenerator.h"

std::atomic<uint64_t> IdGenerator::s_counter = 0;
