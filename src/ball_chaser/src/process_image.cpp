#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool white_detected = false;
    int H = (int)img.height;
    int S = (int)img.step;
    int direction = 0;
    int stepPosition = 0;
    int LEFT = 900;
    int RIGHT = 1500;

    // ROS_INFO("Image height and step - Height:%i, Step:%i", H, S);
    //    Height: 800, Step: 2400, Total 1.92Mn

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    // Loop through each pixel in the image and check if its equal to the white pixel

    for (int i = 0; i < H * S; i++) {
        if (img.data[i] - white_pixel == 0) {
            white_detected = true;
            stepPosition = i % S;
            if (stepPosition <= LEFT){
                direction = 1;
                // Turn Left
                drive_robot(0.5, 0.5);
                break;
            }
            if (stepPosition >= RIGHT){
                direction = 3;
                // Turn Right
                drive_robot(0.5, -0.5);
                break;
            }
            direction = 2;
            // Go Forward
            drive_robot(0.5, 0.0);
            
            break;
        }
    }
    // Stop
    if (direction == 0) {
        drive_robot(0.0, 0.0);
    }
    ROS_INFO("Direction :%i", direction);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
