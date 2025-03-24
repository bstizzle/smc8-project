///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 ** This sample demonstrates how to use the ZED for positional tracking  **
 ** and display camera motion in an OpenGL window. 		                **
 **************************************************************************/

// ZED includes
#include <sl/Camera.hpp>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

// For Foxglove WS:
#include "MCAPWriter.h"

// Sample includes
#include "json.hpp"
#include "Utils.hpp"

// Using std namespace
using namespace std;
using namespace sl;

void displaySLAM(sl::Camera & zed);

int main(int argc, char **argv)
{
    // Open Foxglove socket connection
    mcap_wrapper::open_network_connection("0.0.0.0", 8765, "SLAMViewer");
    
    Camera zed;
    // Set configuration parameters for the ZED
    InitParameters init_parameters;
    init_parameters.coordinate_units = UNIT::METER;
    init_parameters.coordinate_system = COORDINATE_SYSTEM::IMAGE;
    init_parameters.depth_mode = DEPTH_MODE::NEURAL;
    init_parameters.sdk_verbose = true;
    auto mask_path = parseArgs(argc, argv, init_parameters);

    // Open the camera
    auto returned_state = zed.open(init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS)
    {
        print("Camera Open", returned_state, "Exit program.");
        return EXIT_FAILURE;
    }

    // Set parameters for Positional Tracking
    PositionalTrackingParameters positional_tracking_param;  
    positional_tracking_param.enable_imu_fusion = true;
    positional_tracking_param.mode = sl::POSITIONAL_TRACKING_MODE::HYBRID;
    // enable Positional Tracking
    returned_state = zed.enablePositionalTracking(positional_tracking_param);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Enabling positional tracking failed: ", returned_state);
        zed.close();
        return EXIT_FAILURE;
    }
    
    // Main grab loop
    sl::Pose camera_path;
    sl::POSITIONAL_TRACKING_STATE tracking_state;
    while (1)
    {
        if (zed.grab() == ERROR_CODE::SUCCESS)
        {
            // Get the position of the camera in a fixed reference frame (the World Frame)
            tracking_state = zed.getPosition(camera_path, REFERENCE_FRAME::WORLD);
            // Display map and report error if any
            try{
                displaySLAM(zed);
            }
            catch(std::runtime_error & e){
                std::cerr << "Error occured: " << e.what() << std::endl;
            }
        }
        else
            sleep_ms(1);
    }

    zed.disablePositionalTracking();
    zed.close();
    return EXIT_SUCCESS;
}

inline int findImageExtension(int argc, char **argv) {
    int arg_idx=-1;
    int arg_idx_search = 0;
    if (argc > 2) arg_idx_search=2;
    else if(argc > 1) arg_idx_search=1;

    if(arg_idx_search > 0 && (string(argv[arg_idx_search]).find(".png") != string::npos || 
        string(argv[arg_idx_search]).find(".jpg") != string::npos))
        arg_idx = arg_idx_search;
    return arg_idx;
}

std::array<double,4> keyframe_color = {1, 1, 1, 0.2};
std::array<double,4> updated_kf_color = {157 / 255.0, 121 / 255.0, 118 / 255.0, 1.0};
std::array<double,4> keyframe_link_color = {1, 1, 1, 0.8};
std::array<double,4> landmark_color = {217/255.0, 255/255.0, 66/255.0, 0.9};
std::array<double,4> updated_landmark_color = {157 / 255.0, 121 / 255.0, 118 / 255.0, 1.0};


std::vector<sl::Pose> all_poses;

void displaySLAM(sl::Camera & zed){

    // Push image with detected point and landmark:
    sl::Mat left;
    sl::ERROR_CODE error = zed.retrieveImage(left, sl::VIEW::LEFT);
    if(error != sl::ERROR_CODE::SUCCESS)
        throw std::runtime_error("Cannot get image");
    cv::Mat left_cv = slMat2cvMat(left);

    // Push image in 3D view:
    sl::Pose pose_sl;
    zed.getPosition(pose_sl, sl::REFERENCE_FRAME::WORLD);
    all_poses.push_back(pose_sl);
    Eigen::Matrix4f camera_position;
    cvtMatrix4f(pose_sl.pose_data, camera_position);
    mcap_wrapper::add_frame_transform_to_all("left_image_with_kp_pose", std::chrono::system_clock::now().time_since_epoch().count(), "root", "zed_left_cam", camera_position);

    if(error != sl::ERROR_CODE::SUCCESS)
        throw std::runtime_error("Cannot get map object");

    // Create 3D object for display it 
    std::string graph_name = "landmarks";
    mcap_wrapper::create_3D_object(graph_name, "root");

    // Draw landmarks:   
    std::map<uint64_t, sl::Landmark> current_landmarks;
    error = zed.getPositionalTrackingLandmarks(current_landmarks);
    if(error != sl::ERROR_CODE::SUCCESS)
        throw std::runtime_error("Cannot get landmarks");

    for(auto & l: current_landmarks){
        Eigen::Matrix4f landmark_position = Eigen::Matrix4f::Identity();
        landmark_position(0, 3) = l.second.position[0];
        landmark_position(1, 3) = l.second.position[1];
        landmark_position(2, 3) = l.second.position[2];
        mcap_wrapper::add_sphere_to_3d_object(graph_name, landmark_position, {0.03, 0.03, 0.03}, landmark_color);
    }
    // Push 3D map to foxglove
    mcap_wrapper::write_3d_object_to_all(graph_name, std::chrono::system_clock::now().time_since_epoch().count());
    
    
    std::vector<sl::Landmark2D> tracked_image_kp;
    error = zed.getPositionalTrackingLandmarks2D(tracked_image_kp);
    if(error != sl::ERROR_CODE::SUCCESS)
        throw std::runtime_error("Cannot get tracked feature in the image");
   

    Eigen::Vector3d camera_position_world_vector = Eigen::Vector3d::Zero();
    camera_position_world_vector[0] = camera_position(0, 3);
    camera_position_world_vector[1] = camera_position(1, 3);
    camera_position_world_vector[2] = camera_position(2, 3);

    mcap_wrapper::create_3D_object("tracked_image_features", "root");
    for(auto feature: tracked_image_kp){
        cv::circle(left_cv, cv::Point2i(feature.image_position.x, feature.image_position.y), 6, cv::Scalar(178, 142, 255), -1);

        // Push link to between current tracked point and current position of the map
        constexpr std::array<double, 4> link_color = {196 / 255.0, 197 / 255.0, 176 / 255.0, 1.0};
        uint64_t landmark_id = feature.landmark_id;
        if(current_landmarks.count(landmark_id)){
            Eigen::Vector3d landmark_position = Eigen::Vector3d::Zero();
            landmark_position[0] = current_landmarks[landmark_id].position[0];
            landmark_position[1] = current_landmarks[landmark_id].position[1];
            landmark_position[2] = current_landmarks[landmark_id].position[2];

            // Check if landmark is not too far from the camera
            Eigen::Vector3d landmark_ref_to_pose = Eigen::Vector3d::Zero();
            landmark_ref_to_pose[0] = landmark_position[0] - pose_sl.getTranslation().tx;
            landmark_ref_to_pose[1] = landmark_position[1] - pose_sl.getTranslation().ty;
            landmark_ref_to_pose[2] = landmark_position[2] - pose_sl.getTranslation().tz;
            float distance = landmark_ref_to_pose.norm();
            if(distance > 15.0){
                continue;
            }

            Eigen::Matrix4f landmark_link_position = Eigen::Matrix4f::Identity();
            mcap_wrapper::add_line_to_3d_object("tracked_image_features", landmark_link_position, 0.5, true, {camera_position_world_vector, landmark_position}, link_color, {link_color, link_color}, {0, 1});
        }
    }
    mcap_wrapper::write_3d_object_to_all("tracked_image_features", std::chrono::system_clock::now().time_since_epoch().count());

   
    // Push all positions:
    mcap_wrapper::create_3D_object("all_poses", "root");
    constexpr std::array<double, 4> all_pose_link = {64 / 255.0, 163 / 255.0, 255 / 255.0, 1.0};
    for(unsigned int i = 1; i < all_poses.size(); i++){
        Eigen::Vector3d pt0 = Eigen::Vector3d::Zero();
        pt0[0] = all_poses[i-1].getTranslation()[0];
        pt0[1] = all_poses[i-1].getTranslation()[1];
        pt0[2] = all_poses[i-1].getTranslation()[2];

        Eigen::Vector3d pt1 = Eigen::Vector3d::Zero();
        pt1[0] = all_poses[i].getTranslation()[0];
        pt1[1] = all_poses[i].getTranslation()[1];
        pt1[2] = all_poses[i].getTranslation()[2];

        Eigen::Matrix4f landmark_link_position = Eigen::Matrix4f::Identity();
        mcap_wrapper::add_line_to_3d_object("all_poses", landmark_link_position, 2.0, true, {pt0, pt1}, all_pose_link, {all_pose_link, all_pose_link}, {0, 1});
    }
    mcap_wrapper::write_3d_object_to_all("all_poses", std::chrono::system_clock::now().time_since_epoch().count());


    // Push camera calibration for projections:
    double fx = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.fx;
    double fy = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.fy;
    double cx = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.cx;
    double cy = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.cy;
    double k1 = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.disto[0];
    double k2 = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.disto[1];
    double p1 = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.disto[2];
    double p2 = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.disto[3];
    double k3 = zed.getCameraInformation().camera_configuration.calibration_parameters.left_cam.disto[4];
    mcap_wrapper::write_camera_calibration_all("left_cam_calibration",
                                            std::chrono::system_clock::now().time_since_epoch().count(), 
                                            "zed_left_cam",
                                            left_cv.cols, left_cv.rows, "rational_polynomial", 
                                            /*All the values above are fake. Replace it by your real one*/
                                            {0, 0, 0, 0, 0}, {fx, 0, cx, 0, fy, cy, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0, 1}, {fx, 0, cx, 0, 0, fy, cy, 0, 0, 0, 1, 0});

    mcap_wrapper::write_image_to_all("left_image_with_kp", left_cv.clone(), std::chrono::system_clock::now().time_since_epoch().count(), "zed_left_cam");
}