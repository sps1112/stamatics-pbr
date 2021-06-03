#include "math_definitions.h"
#include "camera.h"
#include "stb_image_write.h"
#include "config.h"
#include "integrators.h"
#include "scene.h"

#include <iostream>
#include <vector>

void entry()
{
    using namespace pbr;

    // Setup the camera
    Camera camera;
    camera.position = PBR_CAMERA_POSITION;
    camera.look_at = PBR_CAMERA_LOOKAT;
    camera.fov = PBR_CAMERA_FOV_DEG;
    camera.calculate_basis((double) PBR_OUTPUT_IMAGE_COLUMNS / PBR_OUTPUT_IMAGE_ROWS);

    PathIntegrator integrator;
    integrator.set_scene(&PBR_ACTIVE_SCENE);

    std::vector<Colori> image (PBR_OUTPUT_IMAGE_ROWS * PBR_OUTPUT_IMAGE_COLUMNS);

    UniformRNG rng;

    // Iterate over all rows
#if PBR_USE_THREADS
    #pragma omp parallel for private(rng)
#endif
    for (int row = 0; row < PBR_OUTPUT_IMAGE_ROWS; ++row)
    {
        std::cout << "Processing row " << row << std::endl;

        // Iterate over all cols
        for (int col = 0; col < PBR_OUTPUT_IMAGE_COLUMNS; ++col)
        {

            Colorf color;
            for (int i = 0; i < PBR_SAMPLES_PER_PIXEL; ++i)
            {
                auto sample = rng.sample_disk();

                // Normalize (row + deviation, col + deviation) to (x, y) where x and y are between -1 and 1.
                double x = ((col + sample.x) / PBR_OUTPUT_IMAGE_COLUMNS) * 2 - 1;
                double y = ((row + sample.y) / PBR_OUTPUT_IMAGE_ROWS) * 2 - 1;
                Ray ray = camera.get_ray(x, y);
                color = color + integrator.trace_ray(ray, 0) / (PBR_SAMPLES_PER_PIXEL);
            }

            image[row * PBR_OUTPUT_IMAGE_COLUMNS + col] = to_colori(color);
        }
    }

    // Write to file
    stbi_flip_vertically_on_write(true);
    stbi_write_png(PBR_OUTPUT_IMAGE_NAME, PBR_OUTPUT_IMAGE_COLUMNS, PBR_OUTPUT_IMAGE_ROWS, 4, image.data(), PBR_OUTPUT_IMAGE_COLUMNS * sizeof (Colori));

    // Completed successfully! :)
    std::cout << "All ok!" << std::endl;

}

int main()
{
    try
    {
        entry();
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
