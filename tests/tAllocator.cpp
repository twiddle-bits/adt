/*! tAllocator.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "defs.h"

#include "adt/Allocator.hpp"

void NullAlloc()
{
    adt::NullAllocator a;
    testThat(a.Allocate(0) == 0);
    testThat(a.Allocate(12) == 0);
    a.Deallocate(0);
}

void LibcAlloc()
{
    adt::LibcAllocator a;
    adt::MemAddr p0, p1;
    testThat((p0 = a.Allocate(0)) != 0);
    testThat((p1 = a.Allocate(12)) != 0);
    a.Deallocate(p0);
    a.Deallocate(p1);
}

defineSuite(tallocator_cpp)
{
    addTest(NullAlloc);
    addTest(LibcAlloc);
}
