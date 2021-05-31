#pragma once

#include "math_definitions.h"

/** Structure that represents a camera in the scene. */
struct Camera
{
    /** Camera half field-of-view in degrees. */
    double fov = 45;

    /** Camera position. */
    Vec position;

    /** Point that the camera is focusing on. */
    Vec look_at;

    /*!
     * @brief Get a ray from the camera to point (x, y) on the far plane
     * 
     * @param x X-coordinate, between -1 and 1
     * @param y Y-coordinate, between -1 and 1
     * @return Ray
     */
    Ray get_ray(double x, double y) const
    {
        // TODO: Use the vectors u, v, w to find a ray from the camera to (x, y)
        Ray new_ray;
        new_ray.origin = position;
        Vec destination = w + u * x + v * y;
        new_ray.direction = normalize(destination - position);
        return new_ray;
    }

    void calculate_basis(double aspect_ratio)
    {
        // TODO: Calculate u, v, w using look_at and aspect_ratio
        Vec look_dir = normalize(look_at - position);
        Vec world_up(0, 1, 0);
        Vec camera_right = cross(look_dir, world_up);
        Vec camera_up = cross(look_dir, camera_right);
        double Lx, Ly;
        Lx = tan(fov / 2.0);
        Ly = (1 / aspect_ratio) * Lx;
        w = position + look_dir;
        u = (camera_right * Lx / 2.0);
        v = (camera_up * Ly / 2.0) * (-1.0);
    }

private:
    Vec u, v, w;
};
