#pragma once

#include "math_definitions.h"
#include "config.h"

#include <array>

struct DiscreteSampler
{
    const double weight = 1.0;

    // NOTE: in is w.r.t. rays from the camera
    std::vector<Ray> operator()(const Ray &in, const HitResult &hit) const
    {
        switch (hit.material.type)
        {
        case EMaterialType::DIFFUSE:
        {
            // Add a random offset between [-k/2, k/2] to each component
            double k = PBR_DISCRETE_SAMPLER_DIFFUSE_OFFSET;
            Vec d = reflect(in.direction, hit.normal);
            d.x += ((double)std::rand() / RAND_MAX) * k - (k / 2);
            d.y += ((double)std::rand() / RAND_MAX) * k - (k / 2);
            d.z += ((double)std::rand() / RAND_MAX) * k - (k / 2);

            Ray refl;
            refl.direction = normalize(d);
            refl.origin = hit.point;
            return {refl};
        }

        case EMaterialType::SPECULAR:
        {
            Ray refl;
            refl.direction = reflect(in.direction, hit.normal);
            refl.origin = hit.point;
            return {refl};
        }

        default:
            // return a ray with zero-length direction if we don't want to trace the ray further
            return {Ray{}};
        }
    }
};

// Create a NxNxN cubical grid inside a unit sphere centered at hit.point,
//   where N = PBR_GRID_SAMPLER_SIZE. Join the center to each element of this cube to get
//   the sampled rays.
struct GridSampler
{
    /*
        Dimensions:-
        1. Length of half-diagonal = 1 ==> half_side
        2. Length of diagonal = 2
        3. Length of side = 2/sqrt(3)
        4. Length of half side = 1/sqrt(3)
        5. Number of Subdivisions = PBR_GRID_SAMPLER_SIZE
        6. Length of side of smaller cube = 2/{sqrt(3)*PBR_GRID_SAMPLER_SIZE} ==> h
    */
    // weight = h^3 where h is the side of an element of the grid
    const double weight = std::pow(2.0 / (PBR_GRID_SAMPLER_SIZE * std::sqrt(3)), 3.0);

    // NOTE: in is w.r.t. rays from the camera
    std::vector<Ray> operator()(const Ray &in, const HitResult &hit) const
    {
        const Vec center = hit.point;
        const double half_side = 1.0 / std::sqrt(3);
        const double h = 2.0 / (PBR_GRID_SAMPLER_SIZE * std::sqrt(3));

        // Stores the 8 rays from the center to the vertices on the Cube.
        std::vector<Ray> result(8);
        for (short i = 0; i < 8; ++i)
        {
            // Add + or - half_side to each component
            int kx = (i == 2 || i == 3 || i == 4 || i == 7) ? -1 : 1;
            int ky = (i >= 4) ? -1 : 1;
            int kz = (i == 1 || i == 2 || i == 6 || i == 7) ? -1 : 1;

            result[i].origin = center;
            result[i].direction.x = kx * half_side;
            result[i].direction.y = ky * half_side;
            result[i].direction.z = kz * half_side;
        }

        // Three cube edges as basis (Length = 2 * half_side)
        const Vec AB = result[1].direction - result[0].direction;
        const Vec AD = result[3].direction - result[0].direction;
        const Vec AF = result[5].direction - result[0].direction;

        for (size_t i = 1; i < PBR_GRID_SAMPLER_SIZE; ++i)
        {
            for (size_t j = 1; j < PBR_GRID_SAMPLER_SIZE; ++j)
            {
                for (size_t k = 1; k < PBR_GRID_SAMPLER_SIZE; ++k)
                {
                    // Subdivide cube
                    Vec dir = AB * h * i + AD * h * j + AF * h * k + result[0].direction;

                    // Push ray to result
                    Ray ray{};
                    ray.direction = normalize(dir);
                    ray.origin = center;
                    result.push_back(ray);
                }
            }
        }

        return result;
    }
};

// Simulates perfectly diffuse surfaces.
struct DiffuseBRDF
{
    Colorf operator()(const Ray &in, const HitResult &hit, const Ray &out) const
    {
        double diff = clamp(cosv(out.direction, hit.normal));
        return hit.material.color * diff;
    }
};

// Simulates the Phong model
// https://en.wikipedia.org/wiki/Phong_reflection_model
struct PhongBRDF
{
    Colorf operator()(const Ray &in, const HitResult &hit, const Ray &out) const
    {
        const double kD = (hit.material.type == EMaterialType::SPECULAR) ? 0.05 : 0.95;
        const double kS = 1 - kD;
        const double shininess = 32;

        double diff = clamp(cosv(out.direction, hit.normal));
        double spec = std::pow(
            clamp(dot(
                normalize(reflect(out.direction * -1, hit.normal)),
                normalize(in.direction * -1))),
            shininess);

        return hit.material.color * (kD * diff + kS * spec);
    }
};

// Simulates a discrete BRDF
struct DiscreteBRDF
{
    // NOTE: in and out is w.r.t. rays from the camera
    Colorf operator()(const Ray &in, const HitResult &hit, const Ray &out) const
    {
        switch (hit.material.type)
        {
        case EMaterialType::DIFFUSE:
        {
            double diff = clamp(cosv(out.direction, hit.normal));
            return hit.material.color * diff;
        }

        case EMaterialType::SPECULAR:
        {
            return PBR_COLOR_WHITE;
        }

        default:
            return hit.material.color;
        }
    }
};

template <class Sampler, class BRDF>
struct Integrator
{
    void set_scene(const Scene *scene)
    {
        p_scene = scene;
    }

    /*!
    * @brief Recursively trace the ray.
    * 
    * @param ray Ray that's being traced
    * @return Colorf Output color
    */
    Colorf trace_ray(const Ray &ray, int depth) const
    {
        if (depth >= PBR_MAX_RECURSION_DEPTH)
        {
            return PBR_BACKGROUND_COLOR;
        }

        HitResult hit;
        if (intersect_scene(ray, hit))
        {
            auto samples = sampler(ray, hit);

            Colorf result = Colorf{0.0};
            for (const auto &sample_ray : samples)
            {
                Colorf tr = Colorf{1.0};
                if (sample_ray.direction.len() > PBR_EPSILON)
                {
                    tr = trace_ray(sample_ray, depth + 1);
                }

                Colorf coeff = brdf(ray, hit, sample_ray);
                result = result + coeff * tr * sampler.weight;
            }

            // NOTE: result = emission + \sum_0^n weight * BRDF(in, out) * I(hit, trace)
            //   Change the weight for other methods of integration appropriately
            return result + hit.material.emission;
        }
        else
        {
            // If there's no intersection, return a light blue color
            return PBR_BACKGROUND_COLOR;
        }
    }

private:
    const Scene *p_scene;
    BRDF brdf;
    Sampler sampler;

    bool intersect_scene(const Ray &ray, HitResult &out_hit) const
    {
        // Check if this ray intersects with anything in the scene
        bool does_hit = false;
        HitResult closest_hit;
        for (const auto &actor : *p_scene)
        {
            HitResult hit;
            if (actor.intersect(ray, hit))
            {
                if (does_hit)
                {
                    // Already hit something. Check if this one is closer.
                    if (hit.param < closest_hit.param)
                    {
                        closest_hit = hit;
                    }
                }
                else
                {
                    // First hit. Set closest_hit.
                    closest_hit = hit;
                    does_hit = true;
                }
            }
        }

        out_hit = closest_hit;
        return does_hit;
    }
};
