#include "Triangle.h"
#include "rasterizer.h"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    float rotation_angle_radian = rotation_angle * MY_PI / 180;
    model(0, 0) = cos(rotation_angle_radian);
    model(0, 1) = -sin(rotation_angle_radian);
    model(1, 0) = sin(rotation_angle_radian);
    model(1, 1) = cos(rotation_angle_radian);
    // std::cout<<"Rotate Model: "<<std::endl<<model<<std::endl;
    // Then return it.

    return model;
}


Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.

    // Compute l, r, b, t
    float t = tan((eye_fov * MY_PI / 180) / 2) * fabs(zNear);
    float r = aspect_ratio * t;
    float l = -r;
    float b = -t;

    // Orthographic projection
    // Translate to origin
    Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
    translate(0, 3) = -(r + l) / 2;
    translate(1, 3) = -(t + b) / 2;
    translate(2, 3) = -(zNear + zFar) / 2;

    // Sclae to [-1,1]^3
    Eigen::Matrix4f scale = Eigen::Matrix4f::Identity();
    scale(0, 0) = 2 / (r - l);
    scale(1, 1) = 2 / (t - b);
    scale(2, 2) = 2 / (zNear - zFar);

    // get Orthographic projection
    Eigen::Matrix4f ortho = scale * translate;
    // std::cout<<"Orthographic:"<<std::endl<<ortho<<std::endl;

    // Perspective projection
    // get Matrix_persp2ortho
    Eigen::Matrix4f persp2ortho = Eigen::Matrix4f::Zero();
    persp2ortho(0, 0) = zNear;
    persp2ortho(1, 1) = zNear;
    persp2ortho(2, 2) = zNear + zFar;
    persp2ortho(2, 3) = -zNear * zFar;
    persp2ortho(3, 2) = 1;

    // get Perspective projection
    projection = ortho * persp2ortho;
    // std::cout<<"Perspective:"<<std::endl<<projection<<std::endl;
    // Then return it.

    return projection;
}


int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
