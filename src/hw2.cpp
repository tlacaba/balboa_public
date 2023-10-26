#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

bool pointInTriangle(Vector2 point, Vector2 p0, Vector2 p1, Vector2 p2) {
    Vector2 n01 = Vector2{p1.y - p0.y, p0.x - p1.x};
    Vector2 n12 = Vector2{p2.y - p1.y, p1.x - p2.x};
    Vector2 n20 = Vector2{p0.y - p2.y, p2.x - p0.x};
            
    Vector2 p0q = point - p0;
    Vector2 p1q = point - p1;
    Vector2 p2q = point - p2;

    return (
        (dot(p0q, n01) < 0 && dot(p1q, n12) < 0 && dot(p2q, n20) < 0) || 
        (dot(p0q, n01) > 0 && dot(p1q, n12) > 0 && dot(p2q, n20) > 0)
    );
}

Vector3 pointToProjectedPoint(const Vector3& point) {
    double x = - (point.x / point.z);
    double y = - (point.y / point.z);
    double z = -1;                  // - point.z / point.z = -1, always
    return Vector3(x,y,z);
}

Vector2 pointToImageSpace(const Vector3& point, int width, int height, Real scalingFactor, double aspectRatio) {
    double x = width * (point.x + scalingFactor * aspectRatio) / (2 * scalingFactor * aspectRatio);     // x" = w (x' + sa) / 2sa
    double y = -height * (point.y - scalingFactor) / (2 * scalingFactor);                               // y" = -h (y' - s) / 2s
    return Vector2(x,y);
}

Vector2 pointFromCameraToImage(const Vector3& point, int width, int height, Real scalingFactor, double aspectRatio) {
    return pointToImageSpace(pointToProjectedPoint(point), width, height, scalingFactor, aspectRatio);
}

// Vector3 antiAliasing(const Vector3& pixel, Vector2 p0, Vector2 p1, Vector2 p2) {
//     Vector3 averageColor = Vector3{0,0,0};

//     for (int subPixelY = 0; subPixelY < 4; ++subPixelY) {
//         for (int subPixelX = 0; subPixelX < 4; ++subPixelX) {
//             Vector3 topColor;

//             Vector2 subPixelCenter = Vector2{0.125 + pixel.x + subPixelX * 0.25, 0.125 + pixel.y + subPixelY * 0.25};

//             // I just realized how hard it would be to implement a general use antiAliasing function, so probably just gotta use this as a template rather than a function I call lol.

//             averageColor += topColor;
//         }
//     }

//     averageColor = averageColor * (1.0/16);

//     return averageColor;
// }

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    // fill in background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // going from point to projected point
//std::cout << "Pre-projection: " << p0 << p1 << p2 << std::endl;
//std::cout << "Post-projection: " << pointToProjectedPoint(p0) << pointToProjectedPoint(p1) << pointToProjectedPoint(p2) << std::endl;

    Vector2 imageP0 = pointFromCameraToImage(p0, img.width, img.height, s, (double) img.width / img.height);
    Vector2 imageP1 = pointFromCameraToImage(p1, img.width, img.height, s, (double) img.width / img.height);
    Vector2 imageP2 = pointFromCameraToImage(p2, img.width, img.height, s, (double) img.width / img.height);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // Anti-Aliasing part:

            Vector3 averageColor = Vector3{0,0,0};

            for (int subPixelY = 0; subPixelY < 4; ++subPixelY) {
                for (int subPixelX = 0; subPixelX < 4; ++subPixelX) {
                    Vector3 topColor = Vector3{0.5, 0.5, 0.5};

                    Vector2 subPixelCenter = Vector2{0.125 + x + subPixelX * 0.25, 0.125 + y + subPixelY * 0.25};

                    if (pointInTriangle(subPixelCenter, imageP0, imageP1, imageP2))
                        topColor = color;

                    averageColor += topColor;
                }
            }

            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
        }
    }

    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

