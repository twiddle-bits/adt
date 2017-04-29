/*! FlatHashMap.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef adt_flathashmap_hpp
#define adt_flathashmap_hpp

#include "adt/Allocator.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <utility>

namespace adt
{

namespace detail {

template <class K, class V>
struct FlatPairStorage
{
  public:
    struct ElementType
    {
        K first;
        V second;
    };
  private:
    ElementType * array;
    uint8_t * deleted;
    size_t size;
    Allocator * alloc;
  public:
    FlatPairStorage(Allocator & A) : array(0), deleted(0), size(0), alloc(&A) { }

    FlatPairStorage(FlatPairStorage const &) = delete;

    FlatPairStorage(FlatPairStorage && rhs) : array(rhs.array), deleted(rhs.deleted), size(rhs.size), alloc(rhs.alloc)
    {
        rhs.array = 0;
        rhs.deleted = 0;
        rhs.size = 0;
    }

    FlatPairStorage & operator=(FlatPairStorage && rhs)
    {
        if (this == &rhs) return *this;
        std::swap(rhs.array, array);
        std::swap(rhs.deleted, deleted);
        std::swap(rhs.size, size);
        std::swap(rhs.alloc, alloc);
        return *this;
    }
    
    ~FlatPairStorage()
    {
        Clear();
    }

    bool IsDeleted(size_t i) const
    {
        return deleted[i >> 3] & (1 << (i & 7));
    }

    void SetDeleted(size_t i, bool value)
    {
        uint8_t mask = 1 << (i & 7);
        size_t idx = i >> 3;
        deleted[idx] = (deleted[idx] & ~mask) | (mask * value);
    }

    Allocator & Alloc() const { return *alloc; }

    void Clear()
    {
        if (array)
        {
            for (size_t i = 0; i < size; ++i)
            {
                if (!IsDeleted(i))
                {
                    array[i].first.~K();
                    array[i].second.~V();
                    SetDeleted(i, true);
                }
            }
            alloc->Deallocate(array);
            alloc->Deallocate(deleted);
            array = 0;
            deleted = 0;
        }
    }

    void Reset(size_t n = 0)
    {
        Clear();
        if (n == 0) return;
        array = (ElementType *)alloc->Allocate(n * sizeof(ElementType));
        auto num_bytes = (n + 7) >> 3;
        deleted = (uint8_t *)alloc->Allocate(num_bytes);
        for (size_t i = 0; i < num_bytes; ++i) deleted[i] = 0xFF;
        if (!array || !deleted) assert(false && "Failed to allocate memory");
        size = n;
    }

    size_t Size() const
    {
        return size;
    }

    ElementType * Array() const
    {
        return array;
    }
};

} // namespace detail

template <class K, class V, class H = std::hash<K>, class E = std::equal_to<K>>
class FlatHashMap
{
    detail::FlatPairStorage<K, V> storage;
    size_t curr_size;
    float load_factor;

    void ReserveForSize(size_t n)
    {
        auto cap = (size_t)(n/load_factor);
        storage.Reset(cap);
    }
  public:
    FlatHashMap(Allocator & a, float loadfac = 0.5) : storage(a), curr_size(0), load_factor(loadfac) { }

    void reserve(size_t n) { ReserveForSize(n); }

    using value_type = std::pair<K const, V>;

    using iterator = value_type *;
    using const_iterator = value_type const *;

    size_t capacity() const
    {
        return storage.Size();
    }

    size_t size() const
    {
        return curr_size;
    }

    const_iterator begin() const
    {
        return storage.Array();
    }

    const_iterator end() const
    {
        return 0;
    }

    iterator begin()
    {
        return storage.Array();
    }

    iterator end()
    {
        return 0;
    }

    // iterator erase(iterator where)
    // {
    //     assert(where >= begin() && where < begin() + capacity());
    //     auto & 
    // }

    const_iterator find(K const & k) const
    {
        if (curr_size == 0) return 0;
        auto cap = storage.Size();
        auto hash = H()(k) % cap;
        auto const & eq = E();
        auto array = storage.Array();

        for (size_t num_checked = 0; num_checked < cap; ++num_checked)
        {
            auto idx = (hash + num_checked) % cap;
            if (!storage.IsDeleted(idx) && eq(array[idx].first, k))
            {
                return reinterpret_cast<value_type *>(&array[idx]);
            }
        }
        return 0;
    }

    std::pair<iterator, bool> insert(value_type const & e)
    {
        if (curr_size != 0 && (curr_size + 1 < (size_t)(storage.Size()*load_factor)))
        {
            return InsertNoResize(e);
        }

        FlatHashMap<K, V>  new_map(storage.Alloc());
        TransferElem(new_map);
        std::swap(new_map.storage, storage);
        std::swap(new_map.curr_size, curr_size);
        std::swap(new_map.load_factor, load_factor);

        return InsertNoResize(e);
    }

    std::pair<iterator, bool> insert(value_type && e)
    {
        if (curr_size != 0 && (curr_size + 1 < (size_t)(storage.Size()*load_factor)))
        {
            return InsertNoResize(e);
        }

        FlatHashMap<K, V>  new_map(storage.Alloc());
        TransferElem(new_map);
        std::swap(new_map.storage, storage);
        std::swap(new_map.curr_size, curr_size);
        std::swap(new_map.load_factor, load_factor);

        return InsertNoResize(e);
    }

  private:
    void TransferElem(FlatHashMap<K, V> & new_map)
    {
        new_map.reserve(storage.Size() == 0 ? 6 : storage.Size());
        auto cap = storage.Size();
        auto array = storage.Array();
        for (size_t i = 0; i < cap; ++i)
        {
            if (!storage.IsDeleted(i))
            {
                auto & as_elem = *reinterpret_cast<iterator>(&array[i]);
                new_map.InsertNoResize(std::move(as_elem));
            }
        }
    }

    std::pair<iterator, bool> InsertNoResize(value_type const & e)
    {
        auto cap = storage.Size();
        auto hash = H()(e.first) % cap;
        auto const & eq = E();
        auto array = storage.Array();
        for (size_t num_checked = 0; num_checked < cap; ++num_checked)
        {
            auto idx = (hash + num_checked) % cap;
            if (storage.IsDeleted(idx))
            {
                storage.SetDeleted(idx, false);
                new ((void *)&array[idx].first) K(e.first);
                new ((void *)&array[idx].second) V(e.second);
                ++curr_size;
                return std::make_pair(reinterpret_cast<value_type *>(&array[idx]), true);
            }
            else if (eq(array[idx].first, e.first))
            {
                return std::make_pair(reinterpret_cast<value_type *>(&array[idx]), false);
            }
        }
        assert(false && "Should have inserted an value_type");
        return (end(), false);
    }

    std::pair<iterator, bool> InsertNoResize(value_type && e)
    {
        auto cap = storage.Size();
        auto hash = H()(e.first) % cap;
        auto const & eq = E();
        auto array = storage.Array();
        for (size_t num_checked = 0; num_checked < cap; ++num_checked)
        {
            auto idx = (hash + num_checked) % cap;
            if (storage.IsDeleted(idx))
            {
                storage.SetDeleted(idx, false);
                new ((void *)&array[idx].first) K(std::move(e.first));
                new ((void *)&array[idx].second) V(std::move(e.second));
                ++curr_size;
                return std::make_pair(reinterpret_cast<value_type *>(&array[idx]), true);
            }
            else if (eq(array[idx].first, e.first))
            {
                return std::make_pair(reinterpret_cast<value_type *>(&array[idx]), false);
            }
        }
        assert(false && "Should have inserted an element");
        return (end(), false);
    }
};

} // namespace adt

#endif/*adt_flathashmap_hpp*/
