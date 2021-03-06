#pragma once

#include "task.h"
#include <thread>

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

        fn = std::bind( &Engine::execScript, this, std::placeholders::_1 );
    }

    ~Engine() {
        // Dispose the isolate and tear down V8.
        v8::V8::Dispose();
        v8::V8::ShutdownPlatform();
    }

    void asyncTask( const std::string& js ) {
        std::unique_ptr<ScriptTask> task =
            std::make_unique<ScriptTask>( fn, js );

        platform_.get()->CallOnWorkerThread( std::move( task ) );
    }

    void syncTask( const std::string& js ) {
        std::thread th( &Engine::execScript, this, js );
        th.join();
    }

private:
    std::unique_ptr<v8::Platform> platform_ = nullptr;
    std::function<void( const std::string& js )> fn;

    void execScript( const std::string& script ) {
        // Create a new Isolate and make it the current one.
        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        v8::Isolate* isolate = v8::Isolate::New( create_params );
        {
            v8::Isolate::Scope isolate_scope( isolate );

            // Create a stack-allocated handle scope.
            v8::HandleScope handle_scope( isolate );

            // Create a new context.
            v8::Local<v8::Context> context = v8::Context::New( isolate );

            // Enter the context for compiling and running the hello world script.
            v8::Context::Scope context_scope( context );

            {
                // Create a string containing the JavaScript source code.
                v8::Local<v8::String> source =
                    v8::String::NewFromUtf8( isolate, script.c_str() ).ToLocalChecked();

                // Compile the source code.
                v8::Local<v8::Script> script =
                    v8::Script::Compile( context, source ).ToLocalChecked();

                // Run the script to get the result.
                v8::Local<v8::Value> result = script->Run( context ).ToLocalChecked();

                // Convert the result to an UTF8 string and print it.
                v8::String::Utf8Value utf8( isolate, result );
                printf( "%s\n", *utf8 );
            }
        }

        isolate->Dispose();
        delete create_params.array_buffer_allocator;
    }

};
