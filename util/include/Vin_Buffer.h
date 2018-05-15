//
// Created by cyz on 5/14/18.
//

#ifndef CLOG_VIN_BUFFER_H
#define CLOG_VIN_BUFFER_H

#include <cstring>

namespace vince{
    class Vin_Buffer {
    public:
        Vin_Buffer(){};

        ~Vin_Buffer();

    private:
        Vin_Buffer(const Vin_Buffer&);

        void operator= (const Vin_Buffer&);

    public:

        /**
	 * @brief 将数据放入缓冲
	 *
	 * @param data  要写入的数据起始地址
	 * @param size  要写入的数据字节数
	 * @return      写入的字节数
	 */
        std::size_t PushData(const void* data, std::size_t size);

        /**
	 * @brief 调整缓冲区写游标
	 *
	 * @param bytes 要调整的字节数
	 */
        void Produce(std::size_t bytes) { _writePos += bytes; }

        /**
         * @brief 从缓冲取出数据,深拷贝,调整读游标
         *
         * @param buf   接收数据的起始地址
         * @param size  buf的字节数
         * @return      实际写入的字节数,可能小于size
         */
        std::size_t PopData(void* buf, std::size_t size);

        /**
	 * @brief 从缓冲取出数据,浅拷贝,不调整读游标
	 *
	 * @param buf   接收数据的指针
	 * @param size  接收数据的大小
	 */
        void PeekData(void*& buf, std::size_t& size);

        /**
 * @brief 重设缓冲区指针
 *
 * @param ptr 将要设置的指针
 */
        void ResetBuffer(void* ptr = NULL);
    private:
        /**
         * 读游标
         */
        size_t _readPos;

        /**
         * 写游标
         */
        size_t _writePos;

        /**
         * 缓冲所占用的内存字节数
         */
        size_t _capacity;

        /**
         * 缓冲区
         */
        char* _buffer;

        /**
         * 高水位百分比，影响Shrink()
         */
        size_t _highWaterPercent;
    };
}



#endif //CLOG_VIN_BUFFER_H
