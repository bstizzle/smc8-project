# Tutorial 10: Split Process with the ZED

This tutorial shows how to get an image then the depth from the ZED SDK. The program will loop until 50 frames are grabbed.
We assume that you have followed previous tutorials (opening the ZED, image capture and depth sensing).

### Prerequisites

- Windows 10, Ubuntu LTS, L4T
- [ZED SDK](https://www.stereolabs.com/developers/) and its dependencies ([CUDA](https://developer.nvidia.com/cuda-downloads))

## Build the program

Download the sample and follow the instructions below: [More](https://www.stereolabs.com/docs/getting-started/application-development/)

#### Build for Windows

- Create a "build" folder in the source folder
- Open cmake-gui and select the source and build folders
- Generate the Visual Studio `Win64` solution
- Open the resulting solution and change configuration to `Release`
- Build solution

#### Build for Linux

Open a terminal in the sample directory and execute the following command:

    mkdir build
    cd build
    cmake ..
    make
	
# Code overview
## Create a camera

As in other tutorials, we create, configure and open the ZED.
We set the ZED in HD720 mode at 60fps and enable depth in NEURAL mode. The ZED SDK provides different depth modes: NEURAL_PLUS, NEURAL, NEURAL_LIGHT. For more information, see online documentation.

```
// Create a ZED camera
Camera zed;

// Create configuration parameters
InitParameters init_params;
init_params.sdk_verbose = 1; // Enable the verbose mode
init_params.depth_mode = DEPTH_MODE::NEURAL; // Set the depth mode to NEURAL
init_params.coordinate_units = UNIT::MILLIMETER; // Use millimeter units


// Open the camera
ERROR_CODE err = zed.open(init_params);
if (err != ERROR_CODE::SUCCESS) {
    std::cout << "Error " << err << ", exit program.\n"; // Display the error
    return -1;
}
```

<i>Note: Default parameter for depth mode is DEPTH_MODE::NEURAL. In practice, it is not necessary to set the depth mode in InitParameters. </i>

## Capture data

In previous tutorials, you have seen that images and depth can be captured using the grab method (See Tutorial on Depth Sensing).

It is also possible to first capture the image and then depth. To do so, you can use the read method and then the grab method.

```
// Capture 50 images and depth, then stop
int i = 0;
sl::Mat image, depth;
while (i < 50) {
    // Capture an image
    if (zed.read() == ERROR_CODE::SUCCESS) {
        // A new image is available if read() returns ERROR_CODE::SUCCESS
        zed.retrieveImage(image, VIEW::LEFT); // Get the left image
    }

    // Grab the depth with the previously captured image
    if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {
        zed.retrieveMeasure(depth, MEASURE::DEPTH); // Retrieve depth Mat. Depth is aligned on the left image
        zed.retrieveMeasure(point_cloud, MEASURE::XYZRGBA);
        i++;
    }
}
```

Once 50 frames have been grabbed, we close the camera.

```
// Close the camera
zed.close();
```

You are now using the ZED as a depth sensor.You can move on to the next tutorial to learn how to use the ZED as a positional tracker.

