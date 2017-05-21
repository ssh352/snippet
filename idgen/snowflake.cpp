/*************************************************************************
	> File Name: tmp/snowflake.cpp
	> Author: billowqiu
	> Mail: billowqiu@billowqiu.com 
	> Created Time: 2016-10-25 14:21:52
	> Last Changed: 2016-10-25 14:21:52
*************************************************************************/
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>

class Snowflake
{
public:
    Snowflake(uint64_t worker_id, uint64_t region_id)
    {
        this->worker_id = worker_id;
        this->region_id = region_id;
        this->twepoch = 1288834974657;
        this->last_timestamp = 0;
        this->sequence = 0;
    }

    uint64_t generate(uint64_t bus_id = 0)
    {
        uint64_t timestamp = get_time();
        uint64_t padding_num = this->region_id;
        bool is_padding = (bus_id!=0) ? true : false;
        
        if (is_padding)
        {
            padding_num = bus_id;
        }
        if (timestamp < this->last_timestamp)
        {
            std::cerr << "clock moved backworad!!!" << std::endl;
        }
        if (timestamp == this->last_timestamp)
        {
            this->sequence = (this->sequence+1) & SEQUENCE_MASK;
            if (this->sequence == 0)
            {
                timestamp = tail_next_micros();
            }
        }
        else
        {
            srandom(time(NULL));
            this->sequence = random() % 10;
        }
        this->last_timestamp = timestamp;
        return ((timestamp - this->twepoch) << TIMESTAMP_LEFT_SHIFT |
               (padding_num << REGION_ID_SHIFT) |
               (this->worker_id << WORKER_ID_SHIFT) | this->sequence);

        return 0;
    }

    uint64_t tail_next_micros()
    {
        uint64_t timestamp = get_time();
        while (timestamp <= this->last_timestamp)
        {
            timestamp = get_time();
        }
        return timestamp;
    }

    uint64_t get_time()
    {
        timeval now;
        int ret = gettimeofday(&now, NULL);
        if (-1 == ret)
        {
            exit(-1);
        }
        return now.tv_sec*1000*1000 + now.tv_usec;
    }
private:
    const static uint64_t region_id_bits = 2;
    const static uint64_t worker_id_bits = 10;
    const static uint64_t sequence_bits = 11;

    const static uint64_t MAX_REGION_ID = -1 ^ (-1 << region_id_bits);
    const static uint64_t MAX_WORKER_ID = -1 ^ (-1 << worker_id_bits);
    const static uint64_t SEQUENCE_MASK = -1 ^ (-1 << sequence_bits);

    const static uint64_t WORKER_ID_SHIFT = sequence_bits;
    const static uint64_t REGION_ID_SHIFT = sequence_bits + worker_id_bits;
    const static uint64_t TIMESTAMP_LEFT_SHIFT = (sequence_bits + worker_id_bits + region_id_bits);

    uint64_t twepoch;
    uint64_t last_timestamp;
    uint64_t sequence;
    uint64_t worker_id;
    uint64_t region_id;
};

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: %s uid_count" << argv[0] << std::endl;
        return 0;
    }
    uint32_t uid_count = atoi(argv[1]);
    Snowflake uid(1,0);
    std::ofstream ofs("uidlog");
    timeval begin;
    gettimeofday(&begin, NULL);
    for(uint32_t i=0; i<uid_count; ++i)
    {
        ofs << uid.generate() << std::endl;
    }
    timeval end;
    gettimeofday(&end, NULL);
    ofs << "elapse " << (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000 
        << "ms" << std::endl;
    ofs.close();

    return 0;
}
