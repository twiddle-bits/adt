/*! tFlatHashMap.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "defs.h"

#include "adt/FlatHashMap.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

static adt::LibcAllocator a;

void flatHash()
{
    adt::FlatHashMap<int, int> map(a);
    testThat(map.size() == 0);
    testThat(map.insert({1, 2}).second);
    testThat(map.size() == 1);
    testThat(map.find(1)->second == 2);
}

int N = 1000000;

template <class T>
void mapIntPerf(T & map)
{
    auto t_start = std::chrono::high_resolution_clock::now();
    // million insertions
    for (int i = 0; i < N; ++i)
    {
        map.insert({i, i});
    }
    auto t_insert = std::chrono::high_resolution_clock::now();
    // 2 million finds
    for (int i = 0; i < 2*N; ++i)
    {
        testThat(map.find(i % N)->second == (i % N));
    }
    auto t_find = std::chrono::high_resolution_clock::now();

    std::cout << "\n\tTime taken:\n"
              << "\t- insert: " << std::chrono::duration<double, std::milli>(t_insert-t_start).count() << "ms.\n"
              << "\t- find:   " << std::chrono::duration<double, std::milli>(t_find-t_insert).count() << "ms. ";
}

template <class T>
void mapStringPerf(T & map)
{
    auto t_start = std::chrono::high_resolution_clock::now();
    // million insertions
    for (int i = 0; i < N; ++i)
    {
        map.insert({std::to_string(i), i});
    }
    auto t_insert = std::chrono::high_resolution_clock::now();
    // 2 million finds
    for (int i = 0; i < 2*N; ++i)
    {
        testThat(map.find(std::to_string(i % N))->second == (i % N));
    }
    auto t_find = std::chrono::high_resolution_clock::now();

    std::cout << "\n\tTime taken:\n"
              << "\t- insert: " << std::chrono::duration<double, std::milli>(t_insert-t_start).count() << "ms.\n"
              << "\t- find:   " << std::chrono::duration<double, std::milli>(t_find-t_insert).count() << "ms. ";
}

void flatHashPerf()
{
    adt::FlatHashMap<int, int> map(a);
    mapIntPerf(map);
    adt::FlatHashMap<std::string, int> mapStr(a);
    mapStringPerf(mapStr);
}

void stdMapPerf()
{
    std::map<int, int> map;
    mapIntPerf(map);
    std::map<std::string, int> mapStr;
    mapStringPerf(mapStr);
}

void stdUnordMapPerf()
{
    std::unordered_map<int, int> map;
    mapIntPerf(map);
    std::unordered_map<std::string, int> mapStr;
    mapStringPerf(mapStr);
}

defineSuite(tflathashmap_cpp)
{
    if (auto env = getenv("N"))
    {
        N = atoi(env);
    }
    addTest(flatHash);
    addTest(flatHashPerf);
    addTest(stdMapPerf);
    addTest(stdUnordMapPerf);
}
