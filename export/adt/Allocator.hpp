/*! Allocator.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef adt_allocator_hpp
#define adt_allocator_hpp

#include "adt/exports.h"

#include <cstdint>
#include <cstdlib>

namespace adt
{

using MemAddr = void*;

struct Allocator
{
    virtual MemAddr Allocate(size_t s) = 0;
    virtual void Deallocate(MemAddr p) = 0;
    virtual ~Allocator() = default;
};

struct ADT_API NullAllocator : public Allocator
{
    MemAddr Allocate(size_t s) override;

    void Deallocate(MemAddr p) override;
};

struct ADT_API LibcAllocator : public Allocator
{
    MemAddr Allocate(size_t s) override;

    void Deallocate(MemAddr p) override;
};

} // namespace adt

#endif/*adt_allocator_hpp*/
