#pragma once

#include <functional>

#include "include/v8-platform.h"
#include "waitgroup.h"

class TestTask : public v8::Task {
public:
    explicit TestTask( std::function<void( WaitGroup* )> f, WaitGroup* wg ) : f_( std::move( f ) ), wg_( wg ) {}

    void Run() override { f_( wg_ ); }

private:
    std::function<void( WaitGroup* )> f_;
    WaitGroup* wg_ = nullptr;
};