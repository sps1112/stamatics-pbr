#pragma once

struct Vec
{
    // TODO: What variables do we need to uniquely define a vector in 3D?
    float x;
    float y;
    float z;
};

struct Ray
{
    // TODO: What variables do we need to uniquely define a ray in 3D?
    Vec origin;
    Vec dir;
};

struct Sphere
{
    // TODO: What variables do we need to uniquely define a sphere in 3D?
    Vec origin;
    float radius;
};
