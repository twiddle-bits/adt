/*! FlatHashMap.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef adt_flathashmap_hpp
#define adt_flathashmap_hpp

#include "adt/Allocator.hpp"

#include <cassert>
#include <cstdlib>
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
        K key;
        V value;
        bool deleted;
    };
  private:
    ElementType * array;
    size_t size;
    Allocator * alloc;
  public:
    FlatPairStorage(Allocator & A) : array(0), size(0), alloc(&A) { }

    FlatPairStorage(FlatPairStorage const &) = delete;

    FlatPairStorage(FlatPairStorage && rhs) : array(rhs.array), size(rhs.size), alloc(rhs.alloc)
    {
        rhs.array = 0;
        rhs.size = 0;
    }

    FlatPairStorage & operator=(FlatPairStorage && rhs)
    {
        if (this == &rhs) return *this;
        std::swap(rhs.array, array);
        std::swap(rhs.size, size);
        std::swap(rhs.alloc, alloc);
        return *this;
    }
    
    ~FlatPairStorage()
    {
        Clear();
    }

    Allocator & Alloc() const { return *alloc; }

    void Clear()
    {
        if (array)
        {
            for (size_t i = 0; i < size; ++i)
            {
                if (!array[i].deleted)
                {
                    array[i].key.~K();
                    array[i].value.~V();
                    array[i].deleted = true;
                }
            }
            alloc->Deallocate(array);
        }
    }

    void Reset(size_t n = 0)
    {
        Clear();
        array = (ElementType *)alloc->Allocate(n * sizeof(ElementType));
        size = n;
        for (size_t i = 0; i < n; ++i)
        {
            array[i].deleted = true;
            new ((void *)&array[i].key) K();
            new ((void *)&array[i].value) V();
        }
    }

    size_t Size() const
    {
        return size;
    }

    ElementType * Array() const
    {
        return array;
    }

    void Swap(FlatPairStorage & rhs)
    {
        assert(rhs.alloc == alloc);
        std::swap(array, rhs.array);
        std::swap(size, rhs.size);
    }

    using PairType = std::pair<K const, V>;
};

} // namespace detail

template <class K, class V, class H = std::hash<K>, class E = std::equal_to<K>>
class FlatHashMap
{
    detail::FlatPairStorage<K, V> storage;
    size_t curr_size;
    float load_factor;

    struct Mover
    {
        static V && Do(V & v) { return std::move(v); }
    };

    struct Copier
    {
        static V const & Do(V & v) { return v; }
    };

    void ReserveForSize(size_t n)
    {
        auto cap = (size_t)(n/load_factor);
        storage.Reset(cap);
    }
  public:
    FlatHashMap(Allocator & a) : storage(a), curr_size(0), load_factor(0.75) { }

    void reserve(size_t n) { ReserveForSize(n); }

    using Element = std::pair<K const, V>;

    using iterator = Element *;
    using const_iterator = Element const *;

    size_t size() const
    {
        return curr_size;
    }       
    
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
            if (!array[idx].deleted && eq(array[idx].key, k))
            {
                return reinterpret_cast<Element *>(&array[idx]);
            }
        }
        return 0;
    }

    std::pair<iterator, bool> insert(Element const & e)
    {
        if (curr_size != 0 && (curr_size + 1 < (size_t)(storage.Size()*load_factor)))
        {
            return InsertNoResize(e);
        }

        FlatHashMap<K, V>  new_map(storage.Alloc());
        new_map.reserve(storage.Size() == 0 ? 6 : storage.Size());
        auto cap = storage.Size();
        auto array = storage.Array();
        for (size_t i = 0; i < cap; ++i)
        {
            if (!array[i].deleted)
            {
                auto const & as_elem = *reinterpret_cast<iterator>(&array[i]);
                new_map.InsertNoResize(as_elem);
            }
        }

        std::swap(new_map.storage, storage);
        std::swap(new_map.curr_size, curr_size);
        std::swap(new_map.load_factor, load_factor);

        return InsertNoResize(e);
    }

  private:
    std::pair<iterator, bool> InsertNoResize(Element const & e)
    {
        auto cap = storage.Size();
        auto hash = H()(e.first) % cap;
        auto const & eq = E();
        auto array = storage.Array();
        for (size_t num_checked = 0; num_checked < cap; ++num_checked)
        {
            auto idx = (hash + num_checked) % cap;
            if (array[idx].deleted)
            {
                array[idx].deleted = false;
                array[idx].key = e.first;
                array[idx].value = e.second;
                ++curr_size;
                return std::make_pair(reinterpret_cast<Element *>(&array[idx]), true);
            }
            else if (eq(array[idx].key, e.first))
            {
                return std::make_pair(reinterpret_cast<Element *>(&array[idx]), false);
            }
        }
        assert(false && "Should have inserted an element");
    }
};

} // namespace adt

#endif/*adt_flathashmap_hpp*/
