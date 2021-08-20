// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

int main( int argc, char* argv[] ) {

    Engine* engine = new Engine( argv[0] );

    engine->addTask( "'execute task1'" );
    engine->addTask( "'execute task2'" );

    int c = getchar();

    return 0;
}
