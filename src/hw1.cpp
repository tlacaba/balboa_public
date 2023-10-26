#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};

    // parses arguments
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    // for each pixel
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pixel = Vector2{x + Real(0.5), y + Real(0.5)};

            if (length(pixel - center) <= radius) {
                img(x, y) = color;
            }
            else {
                img(x, y) = Vector3{0.5, 0.5, 0.5};
            }
            
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    // fill background first
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    
    /* I want to do rasteriszation to do acceleration
    for each circle
        for each pixel
            check if pixel is in circle
            overwrite color if the circle is closer
    */
    
    // for each circle
    for (int i = 0; i < scene.objects.size(); ++i) {
        Circle currCircle = scene.objects[i];

        // ACCELERATED
        // Bounding Box
        int lowerY = max(0, (int)currCircle.center.y - (int)currCircle.radius - 5);             // the - 5 is just to make the box bigger just in case there was some stuff with decimal places
        int upperY = min(img.height, (int)currCircle.center.y + (int)currCircle.radius + 5);    // same for the + 5

        int lowerX = max(0, (int)currCircle.center.x - (int)currCircle.radius - 5);
        int upperX = min(img.width, (int)currCircle.center.x + (int)currCircle.radius + 5);

        // for each pixel IN BOUNDING BOX
        for (int y = lowerY; y < upperY; y++) {
            for (int x = lowerX; x < upperX; x++) {
                //Vector2 pixel = Vector2{x, y};
                Vector2 pixel = Vector2{x + Real(0.5), y + Real(0.5)};

                if (length(pixel - currCircle.center) <= currCircle.radius) {
                    img(x, y) = currCircle.color;
                }
            }
        }
        
        // NOT ACCELERATED
        // for each pixel
        // for (int y = 0; y < img.height; y++) {
        //     for (int x = 0; x < img.width; x++) {
        //         Vector2 pixel = Vector2{x, y};

        //         if (length(pixel - currCircle.center) <= currCircle.radius) {
        //             img(x, y) = currCircle.color;
        //         }
        //     }
        // }
    }

    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // color in background first
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    for (int i = 0; i < scene.shapes.size(); ++i) {
        Shape currShape = scene.shapes[i];
//std::cout << i << ": ";

        if (auto *circle = std::get_if<Circle>(&currShape)) {               // if circle
//std::cout << "Circle" << std::endl;
            // BOUNDING BOX
            int lowerY = max(0, (int)circle->center.y - (int)circle->radius - 5);
            int upperY = min(img.height, (int)circle->center.y + (int)circle->radius + 5);

            int lowerX = max(0, (int)circle->center.x - (int)circle->radius - 5);
            int upperX = min(img.width, (int)circle->center.x + (int)circle->radius + 5);

            // for each pixel IN BOUNDING BOX
            for (int y = lowerY; y < upperY; y++) {
                for (int x = lowerX; x < upperX; x++) {
                    //Vector2 pixel = Vector2{x, y};
                    Vector2 pixel = Vector2{x + Real(0.5), y + Real(0.5)};

                    if (length(pixel - circle->center) <= circle->radius) {
                        img(x, y) = circle->color;
                    }
                }
            }

        } else if (auto *rectangle = std::get_if<Rectangle>(&currShape)) {  // if rectangle
//std::cout << "Rectangle" << std::endl;
            for (int y = rectangle->p_min.y; y < rectangle->p_max.y; ++y) {
                for (int x = rectangle->p_min.x; x < rectangle->p_max.x; ++x) {
                    img(x, y) = rectangle->color;
                }
            }

        } else if (auto *triangle = std::get_if<Triangle>(&currShape)) {    // if triangle
//std::cout << "Triangle" << std::endl;
            // NO bounding box, but if there was, you'd go from lowest to greatest x and y

            // e01 = p1-p0; e12 = p2-p1; e20 = p0-p2; 
            // n01.x = e01.y = p1.y-p0.y;
            // n01.y = -e01.x = -(p1.x-p0.x);
            Vector2 n01 = Vector2{triangle->p1.y - triangle->p0.y, triangle->p0.x - triangle->p1.x};
            Vector2 n12 = Vector2{triangle->p2.y - triangle->p1.y, triangle->p1.x - triangle->p2.x};
            Vector2 n20 = Vector2{triangle->p0.y - triangle->p2.y, triangle->p2.x - triangle->p0.x};
            // NOTE: for some reason, the normals are rotated 90 degrees ccw, instead of cw, even though I did the (x,y) -> (y,-x) transformation
            // As a result, I need to flip the comparison on the dot product

// std::cout << "Normals: " << n01 << n12 << n20 << std::endl;

            for (int y = 0; y < img.height; ++y) {
                for (int x = 0; x < img.width; ++x) {
                    //Vector2 pixel = Vector2{x, y};
                    Vector2 pixel = Vector2{x + Real(0.5), y + Real(0.5)};

                    Vector2 p0q = pixel - triangle->p0;
                    Vector2 p1q = pixel - triangle->p1;
                    Vector2 p2q = pixel - triangle->p2;

//std::cout << "Pixel-Edge: " << p0q << p1q << p2q << std::endl;

                    if (
                        (dot(p0q, n01) < 0 && dot(p1q, n12) < 0 && dot(p2q, n20) < 0) || 
                        (dot(p0q, n01) > 0 && dot(p1q, n12) > 0 && dot(p2q, n20) > 0)
                    ) {
                        img(x, y) = triangle->color;
                    }
                }
            }
        }
    }

    return img;
}

// Imma start making stuff easier to read cause imma cry

bool pixelInCircle(Vector2 pixel, Circle circle) {
    return (length(pixel - circle.center) <= circle.radius);
}

bool pixelInRectangle(Vector2 pixel, Rectangle rectangle) {
    return (
        pixel.x >= rectangle.p_min.x && 
        pixel.x <= rectangle.p_max.x &&
        pixel.y >= rectangle.p_min.y && 
        pixel.y <= rectangle.p_max.y
    );
}

bool pixelInTriangle(Vector2 pixel, Triangle triangle) {
    Vector2 n01 = Vector2{triangle.p1.y - triangle.p0.y, triangle.p0.x - triangle.p1.x};
    Vector2 n12 = Vector2{triangle.p2.y - triangle.p1.y, triangle.p1.x - triangle.p2.x};
    Vector2 n20 = Vector2{triangle.p0.y - triangle.p2.y, triangle.p2.x - triangle.p0.x};
            
    Vector2 p0q = pixel - triangle.p0;
    Vector2 p1q = pixel - triangle.p1;
    Vector2 p2q = pixel - triangle.p2;

    return (
        (dot(p0q, n01) < 0 && dot(p1q, n12) < 0 && dot(p2q, n20) < 0) || 
        (dot(p0q, n01) > 0 && dot(p1q, n12) > 0 && dot(p2q, n20) > 0)
    );
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // fill in background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    
    // Shape currShape = scene.shapes[0];

    // if (auto *circle = std::get_if<Circle>(&currShape)) {
    //     std::cout << "Circle!" << std::endl;
    //     std::cout << circle->transform << std::endl;

    //     Matrix3x3 inverseT = inverse(circle->transform);
    //     std::cout << inverseT << std::endl;

    //     Vector3 pixel = {330, 190, 1};
    //     Vector3 inversedPixel = inverseT * pixel;
        
    //     std::cout << inversedPixel << std::endl;

    // } else if (auto *rectangle = std::get_if<Rectangle>(&currShape)) { 
    //     std::cout << "Rectangle!" << std::endl;
    // } else if (auto *triangle = std::get_if<Triangle>(&currShape)) {
    //     std::cout << "Triangle!" << std::endl;
    // }

    for (int i = 0; i < scene.shapes.size(); ++i) {
        Shape currShape = scene.shapes[i];

        if (auto *circle = std::get_if<Circle>(&currShape)) {
            //circle->transform = scale(circle->transform, SCALAR);

//std::cout << "Post-Scale" << circle->transform << std::endl;

            for (int y = 0; y < img.height; ++y) {
                for (int x = 0; x < img.width; ++x) {
                    //Vector3 pixel = Vector3{x,y,1};
                    Vector3 pixel = Vector3{x + Real(0.5), y + Real(0.5), 1.0};
                    Matrix3x3 inverseTransform = inverse(circle->transform);

                    Vector3 pixelInObjectSpace3 = inverseTransform * pixel;
                    Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                    
                    if (pixelInCircle(pixelInObjectSpace2, *circle)) img(x,y) = circle->color;
                }
            }
        } else if (auto *rectangle = std::get_if<Rectangle>(&currShape)) { 
            //rectangle->transform = scale(rectangle->transform, SCALAR);

            for (int y = 0; y < img.height; ++y) {
                for (int x = 0; x < img.width; ++x) {
                    //Vector3 pixel = Vector3{x,y,1};
                    Vector3 pixel = Vector3{x + Real(0.5), y + Real(0.5), 1.0};
                    Matrix3x3 inverseTransform = inverse(rectangle->transform);

                    Vector3 pixelInObjectSpace3 = inverseTransform * pixel;
                    Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                    
                    if (pixelInRectangle(pixelInObjectSpace2, *rectangle)) img(x,y) = rectangle->color;
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&currShape)) {
            //triangle->transform = scale(triangle->transform, SCALAR);

            for (int y = 0; y < img.height; ++y) {
                for (int x = 0; x < img.width; ++x) {
                    //Vector3 pixel = Vector3{x,y,1};
                    Vector3 pixel = Vector3{x + Real(0.5), y + Real(0.5), 1.0};

                    Vector3 pixelInObjectSpace3 = inverse(triangle->transform) * pixel;
                    Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                    
                    if (pixelInTriangle(pixelInObjectSpace2, *triangle)) img(x,y) = triangle->color;
                }
            }
        }
    }

    return img;
// REMINDER TO DO ANIMATION LATER

// Idea: implement a factor in the vector multiplications based on a variable 0-1 that changes the percentage of the translation
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    /* Pseudocode for anti-aliasing
        for each pixel:
            sample the 16 subpixels (4x4) within
                for the top layer object (or background), add color to total (going to make an average)
            take the average by dividing by 16
            color pixel the average
    */
    
    // Raytracing
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {

            Vector3 averageColor = Vector3{0,0,0};

            // This is to get the 16 subpixels
            for (int pixelY = 0; pixelY < 4; ++pixelY) {
                for (int pixelX = 0; pixelX < 4; ++pixelX) {
                    Vector3 pixel = Vector3{0.125 + x + pixelX * 0.25, 0.125 + y + pixelY * 0.25, 1.0};

                    Vector3 topColor = scene.background;

                    for (int i = 0; i < scene.shapes.size(); ++i) {

                        Shape currShape = scene.shapes[i];

                        if (auto *circle = std::get_if<Circle>(&currShape)) {
                            Matrix3x3 inverseTransform = inverse(circle->transform);

                            Vector3 pixelInObjectSpace3 = inverseTransform * pixel;
                            Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                            
                            if (pixelInCircle(pixelInObjectSpace2, *circle)) {
                                topColor = circle->color;
                            }

                        } else if (auto *rectangle = std::get_if<Rectangle>(&currShape)) { 
                            Matrix3x3 inverseTransform = inverse(rectangle->transform);

                            Vector3 pixelInObjectSpace3 = inverseTransform * pixel;
                            Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                            
                            if (pixelInRectangle(pixelInObjectSpace2, *rectangle)) {
                                topColor = rectangle->color;
                            }

                        } else if (auto *triangle = std::get_if<Triangle>(&currShape)) {
                            Vector3 pixelInObjectSpace3 = inverse(triangle->transform) * pixel;
                            Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
                    
                            if (pixelInTriangle(pixelInObjectSpace2, *triangle)) {
                                topColor = triangle->color;
                            }
                        }

                    }

                    averageColor += topColor;
                }
            }
            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
        }
    }
    
    return img;
}

bool isPixelInShape(Vector2 pixel, Shape shape) {

    Vector3 pixel3 = Vector3{pixel.x, pixel.y, 1.0};

    if (auto *circle = std::get_if<Circle>(&shape)) {
        Vector3 pixelInObjectSpace3 = inverse(circle->transform) * pixel3;
        Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
        
        return pixelInCircle(pixelInObjectSpace2, *circle);

    } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) { 
        Vector3 pixelInObjectSpace3 = inverse(rectangle->transform) * pixel3;
        Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};
        
        return pixelInRectangle(pixelInObjectSpace2, *rectangle);

    } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
        Vector3 pixelInObjectSpace3 = inverse(triangle->transform) * pixel3;
        Vector2 pixelInObjectSpace2 = Vector2{pixelInObjectSpace3.x,pixelInObjectSpace3.y};

        return pixelInTriangle(pixelInObjectSpace2, *triangle);
    }

    return false;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // Filling in background
    // for (int y = 0; y < img.height; y++) {
    //     for (int x = 0; x < img.width; x++) {
    //         img(x, y) = scene.background;
    //     }
    // }

    /* Psuedocode for Anti-aliasing
        For each pixel:
            get a total to make an average
            For subpixel (16 total):
                for each shape:
                    get topColor
                add it to average
            divide total by 16 to get average for antialiasing
    */

    /* Psuedocode for Alpha blending
        Start with background color
        For each pixel:
            For each shape:
                If pixel is in shape:
                    img(x,y) = img(x,y) * (1 - alpha) + shape.color * alpha
    */

    /* FUSION
        Start with background color
        For each pixel:
            get a total to make an average
            For subpixel (16 total):
                get top color (background first)
                For each shape:
                    If pixel is in shape:
                        topColor = topColor * (1 - alpha) + shape.color * alpha
                add top color to total
            divide total to get average
    */
    
    // Raytrace
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {

            Vector3 averageColor = Vector3{0,0,0};

            // This is to get the 16 subpixels
            for (int pixelY = 0; pixelY < 4; ++pixelY) {
                for (int pixelX = 0; pixelX < 4; ++pixelX) {
                    Vector2 pixel = Vector2{0.125 + x + pixelX * 0.25, 0.125 + y + pixelY * 0.25};

                    Vector3 topColor = scene.background;

                    for (int i = 0; i < scene.shapes.size(); ++i) {

                        Shape currShape = scene.shapes[i];

                        if (auto *circle = std::get_if<Circle>(&currShape)) {
                            if (isPixelInShape(pixel, currShape)) {
                                topColor = topColor * (1 - circle->alpha) + circle->color * circle->alpha;
                            }

                        } else if (auto *rectangle = std::get_if<Rectangle>(&currShape)) { 
                            if (isPixelInShape(pixel, currShape)) {
                                topColor = topColor * (1 - rectangle->alpha) + rectangle->color * rectangle->alpha;
                            }

                        } else if (auto *triangle = std::get_if<Triangle>(&currShape)) {
                            if (isPixelInShape(pixel, currShape)) {
                                topColor = topColor * (1 - triangle->alpha) + triangle->color * triangle->alpha;
                            }
                        }

                    }

                    averageColor += topColor;
                }
            }
            averageColor = averageColor * (1.0/16);

            img(x,y) = averageColor;
        }
    }

    return img;
}
