#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <time.h>

#define PBR_PI 3.1415926535897932384626433832795
#define PBR_EPSILON 1e-4
#define PBR_DEG_TO_RAD(DEG) (DEG * PBR_PI / 180)

const double point_scaling = 1.0;

// A simple struct to represent a point in 2D
struct Point
{
    double x, y;
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
};

bool is_inside_circle(Point p, double radius)
{
    double distance = sqrt((p.x * p.x) + (p.y * p.y));
    if ((distance - radius) < PBR_EPSILON)
    {
        return true;
    }
    return false;
}

/*!
 * This function returns a collection of randomly sampled points that lie 
 * inside a unit circle.
 * 
 * @param n Number of points to sample
 * @return std::vector<Point> A list of sampled points
 */
std::vector<Point> sample_unit_circle(size_t n)
{
    // TODO: Pick n random points inside the unit circle, any way you want
    std::vector<Point> points;
    srand(time(0));
    bool hasSample = false;
    int validSamples = 0;
    int iteration = 0;
    while (!hasSample)
    {
        Point p;
        p.x = (((double)rand()) / RAND_MAX) * 2.0 - 1.0;
        p.y = (((double)rand()) / RAND_MAX) * 2.0 - 1.0;
        if (is_inside_circle(p, point_scaling))
        {
            points.push_back(p);
            validSamples++;
        }
        iteration++;
        if (validSamples >= n)
        {
            break;
        }
    }
    std::cout << "Total Iterations: " << iteration << std::endl;
    return points;
}

std::vector<Point> sample_unit_circle_center(size_t n)
{
    // TODO: Pick n random points inside the unit circle, any way you want
    std::vector<Point> points;
    srand(time(0));
    bool hasSample = false;
    int validSamples = 0;
    int iteration = 0;
    while (!hasSample)
    {
        double distance = (((double)rand()) / RAND_MAX);
        double theta = 360.0 * (((double)rand()) / RAND_MAX);
        Point p;
        p.x = distance * point_scaling * cos(PBR_DEG_TO_RAD(theta));
        p.y = distance * point_scaling * sin(PBR_DEG_TO_RAD(theta));
        if (is_inside_circle(p, point_scaling))
        {
            points.push_back(p);
            validSamples++;
        }
        iteration++;
        if (validSamples >= n)
        {
            break;
        }
    }
    std::cout << "Total Iterations: " << iteration << std::endl;
    return points;
}
