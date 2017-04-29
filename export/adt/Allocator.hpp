/*! Allocator.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef adt_allocator_hpp
#define adt_allocator_hpp

#include "adt/exports.h"

#include <cstdint>
#include <cstdlib>

namespace adt
{

using MemAddr = void *;

class ADT_API Allocator
{
  public:
    virtual ~Allocator() = default;
    virtual MemAddr Allocate(size_t s) = 0;
    virtual void Deallocate(MemAddr p) = 0;
};

class ADT_API NullAllocator : public Allocator
{
  public:
    MemAddr Allocate(size_t s) override;
    void Deallocate(MemAddr p) override;
};

class ADT_API LibcAllocator : public Allocator
{
  public:
    MemAddr Allocate(size_t s) override;
    void Deallocate(MemAddr p) override;
};

} // namespace adt

#endif/*adt_allocator_hpp*/
