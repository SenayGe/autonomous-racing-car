# Autonomous Racing Car
This project explores some the hardware and software used in self-driving technology by building an autonomous vehicle protoype that mimics, on a racing track, the driving tasks of a full-self driving car on controlled road conditions.
## Description
Placed on a track, the vechicle can detect lanes (marked with white lines) and keep moving without crossing them, which, as in the real world, is very necessary in order  to avoid collision with other vechicles on the road. A camera mounted on the chassis of the car continuously takes pictures of the road where each frame is then converted, by the raspberry pi on board, into Grayscale. This makes it possible to detect and trace the white lines by their level of luminance using OpenCV library and a combination of Canny edge detection and Otsu's thresholding algorithms. Once the lanes are detected, a heading line is calculated with respect to the position of the vehicle inside the track. This heading line is used to determine the direction in which the steering motors should rotate for the car to stay within the lane. In addition, the vehicle can respond to traffic lights as well as perform obstacle detection and avoidance.
## Hardware
*	**Robot Platform**: the chasis of the vehicle
*	**Raspberry Pi**: a master device for main processing
*	**Dfrduino/Arduino**:  slave device to control the sensors and actuators
*	**Ultrasonic sensors**: for obstacle detection
*	**Raspberry Pi camera**: to get visual inputs
## Software
*	**C**: to write the code to be executed by the Arduino
*	**Python**: the language to be used in the Raspberry pi to processes visual and sensory inputs for analysis and decision making 
*	**OpenCV**: open-source library in python for image processing. 
*	**Arduino IDE**: a platform for writing code for the Arduino
## Hardware communication link
![Communication link](https://user-images.githubusercontent.com/68114136/115240619-fa03fe80-a130-11eb-917f-e8ecc5c4c5fd.PNG)
## Image Processing flow-chart
![Image processing](https://user-images.githubusercontent.com/68114136/115241029-67179400-a131-11eb-8ae0-2d34f68bc2ad.PNG) 
## Lane Detection
![Capture](https://user-images.githubusercontent.com/68114136/115241252-a47c2180-a131-11eb-9f41-e3bb1612aa29.PNG)

