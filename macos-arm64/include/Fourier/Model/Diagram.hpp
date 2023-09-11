
#ifndef _MODEL_DISCRETE_DIAGRAM_HPP_
#define _MODEL_DISCRETE_DIAGRAM_HPP_

#include <cstddef>

#define MAX_DIAGRAM_POINTS_COUNT 256
#define MAX_FOURIER_DIAGRAM_POINTS_COUNT 512

struct diagram_point {
    float x;
    float y;
};

struct discrete_diagram {
    diagram_point diagram_points[MAX_DIAGRAM_POINTS_COUNT];
    std::size_t diagram_points_count;
};

struct fourier_diagram {
    diagram_point diagram_points[MAX_FOURIER_DIAGRAM_POINTS_COUNT];
    std::size_t diagram_points_count;
};

#endif
