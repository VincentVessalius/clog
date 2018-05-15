//
// Created by cyz on 5/14/18.
//

#ifndef CLOG_VIN_MEMORYPOOL_H
#define CLOG_VIN_MEMORYPOOL_H

#include <cstddef>
#include <vector>
#include <list>
#include <mutex>

namespace vince {
    struct Vin_Slice {
        explicit Vin_Slice(void *d = NULL, size_t c = 0, size_t l = 0);

        Vin_Slice(const Vin_Slice&)= default;

        void *_data;
        size_t _capacity;
        size_t _len;
    };

    struct MemListType{
        MemListType()= default;

        MemListType(const MemListType& ml){_list=ml._list;};

        std::list<void *> _list;
        std::mutex _lock;
    };

    class Vin_MemoryPool {
    public:
        Vin_MemoryPool(size_t min_block, size_t max_block);

        ~Vin_MemoryPool();

    private:
        Vin_MemoryPool(const Vin_MemoryPool &) = delete;

        Vin_MemoryPool &operator=(const Vin_MemoryPool &)= delete;

    public:

        Vin_Slice Allocate(size_t b_sz);

        void Deallocate(Vin_Slice &s);

        size_t getMaxBytes() const;

        void setMaxBytes(size_t _max_bytes);

        std::string DebugPrint() const;

    private:

        Vin_Slice _Allocate(size_t b_sz_ali);

        size_t _IndexInPool(size_t sz);
    private:
        size_t _min_block;

        size_t _max_block;

        size_t _max_bytes;

        size_t _total_bytes;

        std::vector<MemListType> _mem_pool;
    public:


    };
}

#endif //CLOG_VIN_MEMORYPOOL_H
