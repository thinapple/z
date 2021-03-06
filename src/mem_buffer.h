#ifndef Z_MEM_BUFFER_H__
#define Z_MEM_BUFFER_H__

#include <stdlib.h>
#include <stdint.h>
#include "def.h"

namespace z {
;

class Mempool {
public:
    virtual ~Mempool();

    virtual void* malloc(size_t size);
    virtual void  free(void *ptr);

    virtual void reset();
};

class CacheAppendMempool : public Mempool {
    struct MemBlock {
        uint32_t    size;
        uint32_t    used;
        MemBlock    *next;
        char        data[];
    };
public:
    explicit CacheAppendMempool(uint32_t head_size = DEF_SIZE_PAGE, 
                                uint32_t append_size = DEF_SIZE_PAGE);
    explicit CacheAppendMempool(void *head, 
                                uint32_t head_size, 
                                uint32_t append_size = DEF_SIZE_PAGE);
    ~CacheAppendMempool();

    void* malloc(size_t size);
    void free(void *ptr);
    void reset();
private:
    void alloc_head(uint32_t head_size);
    bool make_space(size_t size);
protected:
    uint32_t        _append_size;
    MemBlock        *_head;
    MemBlock        *_current;
    uint32_t        _own_head:1;
};


class RWBuffer {
    struct BufferBlock {
        BufferBlock     *next;
        char            data[];
    };

    struct BufferOffset {
        BufferBlock     *block;
        uint32_t        offset;
    };
public:
    RWBuffer(Mempool *mempool = nullptr, uint32_t block_size = DEF_SIZE_PAGE);
    ~RWBuffer();

    uint32_t data_size() const;

    int32_t skip(uint32_t bytes);

    int32_t write(const void *buf, uint32_t bytes);
    int32_t read(void *buf, uint32_t bytes, bool inc_pos = true);

    void block_read(void **buf, uint32_t *bytes);
    void block_ref(void **buf, uint32_t *bytes);
private:
    BufferBlock* make_new_block();
    void release_block(BufferBlock *block);
private:
    Mempool         *_mempool;
    uint32_t        _block_size;
    uint32_t        _block_data_size;
    uint32_t        _data_size;
    BufferOffset    _r_pos;
    BufferOffset    _w_pos;
    uint32_t        _own_mempool:1;
};

class BytesQueue {
private:
    Z_DECLARE_COPY_FUNCTIONS(BytesQueue);
public:
    BytesQueue(uint32_t bytes = DEF_SIZE_LONG_PAGE);
    BytesQueue(void *buf, uint32_t bytes);
    ~BytesQueue();

    void *      in_pos();
    uint32_t    in_size() const;
    bool        commit(uint32_t nbytes);

    void *      out_pos();
    uint32_t    out_size() const;
    bool        consume(uint32_t nbytes);

    bool        optimize(uint32_t expected_bytes = 0);
private:
    void        init_qbuffer(void *buf, uint32_t bytes);
    void        reset_qbuffer();
    struct QBuffer {
        char*       buf;
        uint32_t    size;
        uint32_t    in_offset;
        uint32_t    out_offset;
        uint32_t    is_self_allocated:1;
        uint32_t    reserved:31;
    };

    QBuffer         _d;
};

} // namespace z 

#endif
