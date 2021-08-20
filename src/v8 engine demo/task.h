#pragma once

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

class ScriptTask : public v8::Task {
public:
    explicit ScriptTask( const std::string& script ) : script_( script ) {}

    void Run() override {
        execScript();
    }

private:
    std::string script_;

    void execScript() {
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
                    v8::String::NewFromUtf8( isolate, script_.c_str() ).ToLocalChecked();

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