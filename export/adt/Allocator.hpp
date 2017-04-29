/*! Allocator.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef adt_allocator_hpp
#define adt_allocator_hpp

#include <cstdint>

namespace adt
{

using MemAddr = void*;

struct Allocator
{
    virtual MemAddr Allocate(size_t s) = 0;
    virtual void Deallocate(MemAddr p) = 0;
    virtual ~Allocator() = default;
};

struct NullAllocator : public Allocator
{
    MemAddr Allocate(size_t s) override { return 0; }

    void Deallocate(MemAddr p) override { }
};

struct LibcAllocator : public Allocator
{
    MemAddr Allocate(size_t s) override { return ::operator new(s); }

    void Deallocate(MemAddr p) override { ::operator delete(p); }
};

} // namespace adt

#endif/*adt_allocator_hpp*/
