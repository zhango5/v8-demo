// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#include "point.h"

using namespace std;
using namespace v8;

const std::string fileName = "file.js";

// Reads a file into a v8 string.
MaybeLocal<String> ReadFile( Isolate* isolate, const string& name ) {
    FILE* file = fopen( name.c_str(), "rb" );
    if ( file == NULL ) return MaybeLocal<String>();

    fseek( file, 0, SEEK_END );
    size_t size = ftell( file );
    rewind( file );

    std::unique_ptr<char> chars( new char[size + 1] );
    chars.get()[size] = '\0';
    for ( size_t i = 0; i < size;) {
        i += fread( &chars.get()[i], 1, size - i, file );
        if ( ferror( file ) ) {
            fclose( file );
            return MaybeLocal<String>();
        }
    }
    fclose( file );
    MaybeLocal<String> result = String::NewFromUtf8(
        isolate, chars.get(), NewStringType::kNormal, static_cast<int>(size) );
    return result;
}

void LogCallback( const v8::FunctionCallbackInfo<v8::Value>& info ) {
    if ( info.Length() < 1 ) return;
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope scope( isolate );
    v8::Local<v8::Value> val = info[0];
    v8::String::Utf8Value utf8( isolate, val );
    printf( "%s\n", *utf8 );
}

int main( int argc, char* argv[] ) {
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation( argv[0] );
    v8::V8::InitializeExternalStartupData( argv[0] );
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform( platform.get() );
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New( create_params );
    {
        v8::Isolate::Scope isolate_scope( isolate );

        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope( isolate );

        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New( isolate );
        global->Set( isolate, "log",
            v8::FunctionTemplate::New( isolate, LogCallback ) );

        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New( isolate, nullptr, global );

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope( context );

        {
            // Create a string containing the JavaScript source code.
            v8::Local<v8::String> source;
            if ( !ReadFile( isolate, fileName ).ToLocal( &source ) ) {
                fprintf( stderr, "Error reading '%s'.\n", fileName.c_str() );
                return -1;
            }

            // Compile the source code.
            v8::Local<v8::Script> script =
                v8::Script::Compile( context, source ).ToLocalChecked();

            // Run the script to get the result.
            v8::Local<v8::Value> result = script->Run( context ).ToLocalChecked();

            if ( result.IsEmpty() ) {
                // Convert the result to an UTF8 string and print it.
                v8::String::Utf8Value utf8( isolate, result );
                printf( "%s\n", *utf8 );
            }
        }
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}
