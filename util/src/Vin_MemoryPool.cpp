//
// Created by cyz on 5/14/18.
//

#include <iomanip>
#include <sstream>
#include <zconf.h>
#include <iostream>
#include "util/include/Vin_MemoryPool.h"

namespace vince{

    inline static size_t RoundUp2Power(size_t bytes){
        if(bytes==0||(bytes&(bytes-1))==0)
            return bytes;
        size_t tmp=0;
        while(bytes!=0){
            tmp=bytes;
            bytes&=(bytes-1);
        }
        tmp<<=1;
        return tmp;
    }

////////////////////////////////////////////////////////////////
//cyz-> Implementation of Vin_Slice
    vince::Vin_Slice::Vin_Slice(void *d, size_t c, size_t l):_data(d),_capacity(c),_len(l){

    }

////////////////////////////////////////////////////////////////
//cyz-> Implementation of Vin_MemoryPool
    Vin_MemoryPool::Vin_MemoryPool(size_t min_block, size_t max_block) :
            _min_block(RoundUp2Power(min_block)),_max_block(RoundUp2Power(max_block>min_block?max_block:min_block)),
            _max_bytes(1024*1024),_total_bytes(0){
        size_t now=_min_block;
        while(now<=_max_block){
            MemListType tmp;
            _mem_pool.push_back(tmp);
            now<<=1;
        }
        _mem_pool.shrink_to_fit();
    }

    Vin_MemoryPool::~Vin_MemoryPool() {
        for(auto & iter:_mem_pool){
            for(auto & jter: iter._list){
                delete[] reinterpret_cast<char*>(jter);
            }
        }
    }

    void Vin_MemoryPool::setMaxBytes(size_t max_bytes) {
        _max_bytes = RoundUp2Power(max_bytes);
    }

    size_t Vin_MemoryPool::getMaxBytes() const {
        return _max_bytes;
    }

    Vin_Slice Vin_MemoryPool::Allocate(size_t b_sz) {
        Vin_Slice ret;

        if(b_sz==0)
            return ret;

        size_t b_sz_ali=RoundUp2Power(b_sz);

        if(b_sz_ali<_min_block||b_sz_ali>_max_block){
            ret._data=new char[b_sz_ali];
            ret._capacity=b_sz_ali;
            return ret;
        }else{
            return _Allocate(b_sz_ali);
        }

    }

    void Vin_MemoryPool::Deallocate(Vin_Slice &s) {
        Vin_Slice z;
        if(s._capacity<_min_block||s._capacity>_max_block||s._capacity+_total_bytes>_max_bytes){
            delete[] reinterpret_cast<char*>(s._data);
            s=z;
            return;
        }else{
            size_t mempool_idx=_IndexInPool(s._capacity);
            std::unique_lock<std::mutex> _lck(_mem_pool[mempool_idx]._lock);
            _mem_pool[mempool_idx]._list.push_back(s._data);
            _total_bytes+=s._capacity;
            s=z;
            return;
        }
    }

    std::string Vin_MemoryPool::DebugPrint() const
    {
        std::ostringstream oss;

        oss << "\n===============================================================\n";
        oss << "============  BucketCount " << std::setiosflags(std::ios::left) << std::setw(4) << _mem_pool.size() << " ================================" << std::endl;
        oss << "============  PoolBytes " << std::setw(10) << _total_bytes << " ============================" << std::endl;

        int bucket = 0;
        size_t size = _min_block;

        for (auto it:_mem_pool)
        {
            oss << "== Bucket " << std::setw(3) << bucket
                << ": BlockSize " << std::setw(8) << size
                << " Remain blocks " << std::setw(6) << it._list.size()
                << " ======== \n";
            ++ bucket;
            size *= 2;
        }

        return oss.str();
    }

    Vin_Slice Vin_MemoryPool::_Allocate(size_t b_sz_ali) {
        size_t mempool_idx=_IndexInPool(b_sz_ali);
        Vin_Slice ret;
        std::unique_lock<std::mutex> _lck(_mem_pool[mempool_idx]._lock);
        if(_mem_pool[mempool_idx]._list.empty()){
            ret._data=new char[b_sz_ali];
            ret._capacity=b_sz_ali;
        }else{
            ret._data=*(_mem_pool[mempool_idx]._list.begin());
            ret._capacity=b_sz_ali;
            _mem_pool[mempool_idx]._list.pop_front();
            _total_bytes-=b_sz_ali;
        }
        return ret;
    }

    size_t Vin_MemoryPool::_IndexInPool(size_t sz) {
        size_t mempool_idx=0;
        size_t x=sz/_min_block;
        while(x!=1){
            x>>=1;
            mempool_idx++;
        }
        return mempool_idx;
    }
}

