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
    //UNUSED(mesh); // silence warning, feel free to remove this

    /* PSEUDOCODE FOR 3D RENDERER
        Z_buffer = Image(w,h);
        for each pixel:
            initialize with max_value
        for each triangle:
            project triangle
            for each pixel:
                check if pixel center hits triangle
                overwrite color and Z_buffer if triangle is closer
    */

    /* PSEUDOCODE FOR BARY MADNESS
        for each triangle:
            get projected vertices (camera to image space)
            for each pixel:
                if pixel is in projected vertices:
                    do cross products to get areas
                    with areas, calculate projected bary coords (equation 5)
                    calculate original bary coords (equation 14)
                    get Z with interpolation (equation 15)
                    if Z is beyond close clipping range and closer than the current Z_buffer:
                        update Z_buffer
                        overwrite pixel color
    */

    // YES ANTI-ALIASING
    // Z_buffer = Image(w,h);
    //     for each pixel:
    //         initialize with max_value
    Image1 Z_buffer = Image1(img.width * 4, img.height * 4);

    Image3 highSampleImg = Image3(img.width * 4, img.height *4);

    for (int y = 0; y < highSampleImg.height; y++) {
        for (int x = 0; x < highSampleImg.width; x++) {
            highSampleImg(x, y) = Vector3{0.5,0.5,0.5};
            Z_buffer(x,y) = 10e6;                           // this will be positive, and for comparisons, we will be flipping values accordingly
        }
    }

    double aspectRatio = (double) img.width / img.height;

    // for each triangle
    for (int i = 0; i < mesh.faces.size(); ++i) {
        // project triangle
        Vector3 vertexIndices = mesh.faces[i];

        Vector3 p0 = mesh.vertices[vertexIndices.x];
        Vector3 p1 = mesh.vertices[vertexIndices.y];
        Vector3 p2 = mesh.vertices[vertexIndices.z];
        
        Vector2 projP0 = pointFromCameraToImage(p0, img.width, img.height, s, aspectRatio);
        Vector2 projP1 = pointFromCameraToImage(p1, img.width, img.height, s, aspectRatio);
        Vector2 projP2 = pointFromCameraToImage(p2, img.width, img.height, s, aspectRatio);

        // for each pixel
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                
                for (int subY = 0; subY < 4; ++subY) {
                    for (int subX = 0; subX < 4; ++subX) {
                        Vector2 subPixelCenter = Vector2{0.125 + x + subX * 0.25, 0.125 + y + subY * 0.25};

                        // if pixel in triangle
                        if (pointInTriangle(subPixelCenter, projP0, projP1, projP2)) {
                            // Calculate Z-depth: 
                                // Calculating projected bary coords:
                                    // Converting points to points in 3D (z = 0) for doing cross product to get area
                            Vector3 subPixelCenter3 = Vector3{subPixelCenter.x, subPixelCenter.y, 0.0};
                            Vector3 projP0_3 = Vector3{projP0.x, projP0.y, 0.0};
                            Vector3 projP1_3 = Vector3{projP1.x, projP1.y, 0.0};
                            Vector3 projP2_3 = Vector3{projP2.x, projP2.y, 0.0};

                                        // don't need to divide by 2 since we're checking relative size
                            double totalArea = length(cross(projP1_3 - projP0_3, projP2_3 - projP0_3));
                            double areaP12 = length(cross(projP1_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                            double area0P2 = length(cross(projP0_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                            double area01P = length(cross(projP0_3 - subPixelCenter3, projP1_3 - subPixelCenter3));

                                    // Getting projected bary coords!
                            double projB0 = areaP12 / totalArea;
                            double projB1 = area0P2 / totalArea;
                            double projB2 = area01P / totalArea;

                                // Calculating original bary coords:
                            double bDenominator = projB0 / p0.z + projB1 / p1.z + projB2 / p2.z;
                            double b0 = (projB0 / p0.z) / bDenominator;
                            double b1 = (projB1 / p1.z) / bDenominator;
                            double b2 = (projB2 / p2.z) / bDenominator;

                            // Getting Z!
                            double Z = b0 * p0.z + b1 * p1.z + b2 * p2.z;

                            // if Z is beyond the clipping range and is closer than the current Z_buffer:
                            if (z_near < -Z && -Z < Z_buffer(4 * x + subX, 4 * y + subY)) {
                                // update Z_buffer
                                Z_buffer(4 * x + subX, 4 * y + subY) = -Z;
                                // overwrite pixel color
                                highSampleImg(4 * x + subX, 4 * y + subY) = mesh.face_colors[i];
                            }

                        }


                    }
                }

            }
        }

    }
    
    // Anti-aliasing averaging
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            
            Vector3 averageColor = Vector3{0,0,0};

            for (int subY = 0; subY < 4; ++subY) {
                for (int subX = 0; subX < 4; ++subX) {
                    averageColor += highSampleImg(4 * x + subX, 4 * y + subY);
                }
            }

            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
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
    //UNUSED(mesh); // silence warning, feel free to remove this

    // YES ANTI-ALIASING
    // Z_buffer = Image(w,h);
    //     for each pixel:
    //         initialize with max_value
    Image1 Z_buffer = Image1(img.width * 4, img.height * 4);

    Image3 highSampleImg = Image3(img.width * 4, img.height *4);

    for (int y = 0; y < highSampleImg.height; y++) {
        for (int x = 0; x < highSampleImg.width; x++) {
            highSampleImg(x, y) = Vector3{0.5,0.5,0.5};
            Z_buffer(x,y) = 10e6;                           // this will be positive, and for comparisons, we will be flipping values accordingly
        }
    }

    double aspectRatio = (double) img.width / img.height;

    // for each triangle
    for (int i = 0; i < mesh.faces.size(); ++i) {
        // project triangle
        Vector3 vertexIndices = mesh.faces[i];

        Vector3 p0 = mesh.vertices[vertexIndices.x];
        Vector3 p1 = mesh.vertices[vertexIndices.y];
        Vector3 p2 = mesh.vertices[vertexIndices.z];
        
        Vector2 projP0 = pointFromCameraToImage(p0, img.width, img.height, s, aspectRatio);
        Vector2 projP1 = pointFromCameraToImage(p1, img.width, img.height, s, aspectRatio);
        Vector2 projP2 = pointFromCameraToImage(p2, img.width, img.height, s, aspectRatio);

        // for each pixel
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                
                for (int subY = 0; subY < 4; ++subY) {
                    for (int subX = 0; subX < 4; ++subX) {
                        Vector2 subPixelCenter = Vector2{0.125 + x + subX * 0.25, 0.125 + y + subY * 0.25};

                        // if pixel in triangle
                        if (pointInTriangle(subPixelCenter, projP0, projP1, projP2)) {
                            // Calculate Z-depth: 
                                // Calculating projected bary coords:
                                    // Converting points to points in 3D (z = 0) for doing cross product to get area
                            Vector3 subPixelCenter3 = Vector3{subPixelCenter.x, subPixelCenter.y, 0.0};
                            Vector3 projP0_3 = Vector3{projP0.x, projP0.y, 0.0};
                            Vector3 projP1_3 = Vector3{projP1.x, projP1.y, 0.0};
                            Vector3 projP2_3 = Vector3{projP2.x, projP2.y, 0.0};

                                        // don't need to divide by 2 since we're checking relative size
                            double totalArea = length(cross(projP1_3 - projP0_3, projP2_3 - projP0_3));
                            double areaP12 = length(cross(projP1_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                            double area0P2 = length(cross(projP0_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                            double area01P = length(cross(projP0_3 - subPixelCenter3, projP1_3 - subPixelCenter3));

                                    // Getting projected bary coords!
                            double projB0 = areaP12 / totalArea;
                            double projB1 = area0P2 / totalArea;
                            double projB2 = area01P / totalArea;

                                // Calculating original bary coords:
                            double bDenominator = projB0 / p0.z + projB1 / p1.z + projB2 / p2.z;
                            double b0 = (projB0 / p0.z) / bDenominator;
                            double b1 = (projB1 / p1.z) / bDenominator;
                            double b2 = (projB2 / p2.z) / bDenominator;

                            // Getting Z!
                            double Z = b0 * p0.z + b1 * p1.z + b2 * p2.z;

                            // if Z is beyond the clipping range and is closer than the current Z_buffer:
                            if (z_near < -Z && -Z < Z_buffer(4 * x + subX, 4 * y + subY)) {
                                // update Z_buffer
                                Z_buffer(4 * x + subX, 4 * y + subY) = -Z;
                                // overwrite pixel color
                                highSampleImg(4 * x + subX, 4 * y + subY) = 
                                    b0 * mesh.vertex_colors[vertexIndices.x] + 
                                    b1 * mesh.vertex_colors[vertexIndices.y] + 
                                    b2 * mesh.vertex_colors[vertexIndices.z];
                            }

                        }


                    }
                }

            }
        }

    }
    
    // Anti-aliasing averaging
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            
            Vector3 averageColor = Vector3{0,0,0};

            for (int subY = 0; subY < 4; ++subY) {
                for (int subX = 0; subX < 4; ++subX) {
                    averageColor += highSampleImg(4 * x + subX, 4 * y + subY);
                }
            }

            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
        }
    }

    return img;
}

//===================================================================================================================//

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
            img(x, y) = scene.background;
        }
    }

    // Copy-pasted from 2_3
    Image1 Z_buffer = Image1(img.width * 4, img.height * 4);

    Image3 highSampleImg = Image3(img.width * 4, img.height *4);

    for (int y = 0; y < highSampleImg.height; y++) {
        for (int x = 0; x < highSampleImg.width; x++) {
            highSampleImg(x, y) = scene.background;
            Z_buffer(x,y) = 10e6;                           // this will be positive, and for comparisons, we will be flipping values accordingly
        }
    }

    double aspectRatio = (double) img.width / img.height;

    Matrix4x4 viewMatrix = inverse(scene.camera.cam_to_world);

    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;

    // for each mesh
    for (int j = 0; j < scene.meshes.size(); ++j) {
        TriangleMesh mesh = scene.meshes[j];

        // for each triangle
        for (int i = 0; i < mesh.faces.size(); ++i) {
            // project triangle
            Vector3 vertexIndices = mesh.faces[i];

            Vector3 p0Obj = mesh.vertices[vertexIndices.x];
            Vector3 p1Obj = mesh.vertices[vertexIndices.y];
            Vector3 p2Obj = mesh.vertices[vertexIndices.z];

            Vector4 p0Obj4 = Vector4{p0Obj.x, p0Obj.y, p0Obj.z, 1.0};   // IS IT 1 or 0???
            Vector4 p1Obj4 = Vector4{p1Obj.x, p1Obj.y, p1Obj.z, 1.0};
            Vector4 p2Obj4 = Vector4{p2Obj.x, p2Obj.y, p2Obj.z, 1.0};

            Vector4 p0_4 = viewMatrix * mesh.model_matrix * p0Obj4;
            Vector4 p1_4 = viewMatrix * mesh.model_matrix * p1Obj4;
            Vector4 p2_4 = viewMatrix * mesh.model_matrix * p2Obj4;

            Vector3 p0 = Vector3{p0_4.x, p0_4.y, p0_4.z};
            Vector3 p1 = Vector3{p1_4.x, p1_4.y, p1_4.z};
            Vector3 p2 = Vector3{p2_4.x, p2_4.y, p2_4.z};

            // Projection matrix equivalent (Camera to Scene)
            Vector2 projP0 = pointFromCameraToImage(p0, img.width, img.height, s, aspectRatio);
            Vector2 projP1 = pointFromCameraToImage(p1, img.width, img.height, s, aspectRatio);
            Vector2 projP2 = pointFromCameraToImage(p2, img.width, img.height, s, aspectRatio);

            // for each pixel
            for (int y = 0; y < img.height; ++y) {
                for (int x = 0; x < img.width; ++x) {
                    
                    for (int subY = 0; subY < 4; ++subY) {
                        for (int subX = 0; subX < 4; ++subX) {
                            Vector2 subPixelCenter = Vector2{0.125 + x + subX * 0.25, 0.125 + y + subY * 0.25};

                            // if pixel in triangle
                            if (pointInTriangle(subPixelCenter, projP0, projP1, projP2)) {
                                // Calculate Z-depth: 
                                    // Calculating projected bary coords:
                                        // Converting points to points in 3D (z = 0) for doing cross product to get area
                                Vector3 subPixelCenter3 = Vector3{subPixelCenter.x, subPixelCenter.y, 0.0};
                                Vector3 projP0_3 = Vector3{projP0.x, projP0.y, 0.0};
                                Vector3 projP1_3 = Vector3{projP1.x, projP1.y, 0.0};
                                Vector3 projP2_3 = Vector3{projP2.x, projP2.y, 0.0};

                                            // don't need to divide by 2 since we're checking relative size
                                double totalArea = length(cross(projP1_3 - projP0_3, projP2_3 - projP0_3));
                                double areaP12 = length(cross(projP1_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                                double area0P2 = length(cross(projP0_3 - subPixelCenter3, projP2_3 - subPixelCenter3));
                                double area01P = length(cross(projP0_3 - subPixelCenter3, projP1_3 - subPixelCenter3));

                                        // Getting projected bary coords!
                                double projB0 = areaP12 / totalArea;
                                double projB1 = area0P2 / totalArea;
                                double projB2 = area01P / totalArea;

                                    // Calculating original bary coords:
                                double bDenominator = projB0 / p0.z + projB1 / p1.z + projB2 / p2.z;
                                double b0 = (projB0 / p0.z) / bDenominator;
                                double b1 = (projB1 / p1.z) / bDenominator;
                                double b2 = (projB2 / p2.z) / bDenominator;

                                // Getting Z!
                                double Z = b0 * p0.z + b1 * p1.z + b2 * p2.z;

                                // if Z is beyond the clipping range and is closer than the current Z_buffer:
                                if (z_near < -Z && -Z < Z_buffer(4 * x + subX, 4 * y + subY)) {
                                    // update Z_buffer
                                    Z_buffer(4 * x + subX, 4 * y + subY) = -Z;
                                    // overwrite pixel color
                                    highSampleImg(4 * x + subX, 4 * y + subY) = 
                                        b0 * mesh.vertex_colors[vertexIndices.x] + 
                                        b1 * mesh.vertex_colors[vertexIndices.y] + 
                                        b2 * mesh.vertex_colors[vertexIndices.z];
                                }

                            }


                        }
                    }

                }
            }

        }
    }
        
    
    // Anti-aliasing averaging
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            
            Vector3 averageColor = Vector3{0,0,0};

            for (int subY = 0; subY < 4; ++subY) {
                for (int subX = 0; subX < 4; ++subX) {
                    averageColor += highSampleImg(4 * x + subX, 4 * y + subY);
                }
            }

            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
        }
    }

    return img;
}

