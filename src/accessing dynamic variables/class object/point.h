#pragma once

class Point
{
public:
    Point( int x, int y ) : x_( x ), y_( y ) { }
    int x_, y_;

    int Add() { return x_ + y_; }
};