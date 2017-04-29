/*! Allocator.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "adt/Allocator.hpp"

namespace adt
{

MemAddr NullAllocator::Allocate(size_t s) { return 0; }

void NullAllocator::Deallocate(MemAddr p) { }

MemAddr LibcAllocator::Allocate(size_t s) { return ::operator new(s); }

void LibcAllocator::Deallocate(MemAddr p) { ::operator delete(p); }

} // namespace adt
