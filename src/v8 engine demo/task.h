#pragma once

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#include <functional>

class ScriptTask : public v8::Task {
public:
    explicit ScriptTask( std::function<void( const std::string& js )> fn, const std::string& script )
        : fn_( fn ), script_( script ) {}

    void Run() override {
        fn_( script_ );
    }

private:
    std::string script_;
    std::function<void( const std::string& js )> fn_;
};