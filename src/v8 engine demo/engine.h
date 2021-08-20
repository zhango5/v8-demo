#pragma once

#include "task.h"

class Engine
{
public:
    explicit Engine( const std::string& app ) {
        // Initialize V8.
        v8::V8::InitializeICUDefaultLocation( app.c_str() );
        v8::V8::InitializeExternalStartupData( app.c_str() );
        platform_ = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform( platform_.get() );
        v8::V8::Initialize();
    }

    ~Engine() {
        // Dispose the isolate and tear down V8.
        v8::V8::Dispose();
        v8::V8::ShutdownPlatform();
    }

    void addTask( const std::string& js ) {
        std::unique_ptr<ScriptTask> task =
            std::make_unique<ScriptTask>( js );

        platform_.get()->CallOnWorkerThread( std::move( task ) );
    }

private:
    std::unique_ptr<v8::Platform> platform_ = nullptr;
};
