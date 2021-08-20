#pragma once

#include <atomic>
#include <chrono>
#include <thread>

class WaitGroup
{
public:
    WaitGroup() {}

    void Add( unsigned int cnt ) { count_ += cnt; }
    void Done() { if ( count_ == 0 ) return;  --count_; }

    void Wait() {
        while ( count_ > 0 )
            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
    }

private:
    std::atomic_uint count_ = 0;
};