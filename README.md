# Virtual Scene Modeling of Robot with OpenGL

This repository showcases the virtual scene modeling of a robot using OpenGL. It demonstrates various techniques and features for creating realistic and interactive virtual environments.

## 1. Overview

In this project, I utilize the power of OpenGL to create a virtual scene where a robot is modeled and can be manipulated. The scene includes dynamic lighting, texture mapping, and realistic shaders to enhance the visual experience.

![Screenshot 1](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Schematic_Diagram_of_Model_Construction.png)
*Schematic Diagram of Model Construction*

![Screenshot 1](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Hierarchical_Modeling_Structure_Diagram.png)
*Hierarchical Modeling Structure Diagram*

![Screenshot 1](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Skybox.png)
*Skybox*


## 2. Features

- Real-time rendering and shading
- Dynamic lighting effects
- Texture mapping for realistic surface appearance
- User interaction and manipulation of the robot
- Detailed documentation and code explanations

## 3. Screenshots

![Screenshot 1](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/scene_3.png)
*Program Scene 1*

![Screenshot 2](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/scene_2.png)
*Program Scene 2*

![Screenshot 3](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/scene_1.png)
*Program Scene 3*

## 4. Interactions and Usage Instructions

![Figure 4: Keyboard Key Interactions Diagram](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Keyboard_Interactions_Instructions.png)

### 4.1 Camera Free Movement
- Mouse Movement: Control the camera view direction by moving the mouse.
- Mouse Scroll: Zoom in and out for the camera.
- Keyboard Interaction: 
  - W: Move the camera forward.
  - S: Move the camera backward.
  - A: Move the camera left.
  - D: Move the camera right.

### 4.2 Model Movement
- Keyboard Interaction:
  - Arrow Up: Move the model forward.
  - Arrow Down: Move the model backward.
  - Arrow Left + Arrow Up: Move the model forward and turn left.
  - Arrow Right + Arrow Up: Move the model forward and turn right.
  - Arrow Left + Arrow Down: Move the model backward and turn left.
  - Arrow Right + Arrow Down: Move the model backward and turn right.
  - F: Transform the model from a car to a robot.
  - G: Transform the model from a robot to a car.
  - I: Move model parts.
  - O: Move model parts.


### 4.3 Program Exit
- Keyboard Interaction: Press the "esc" key or press the "B" key to exit the program after starting.
- Easter Egg Interaction: When the model leaves the starting point, press the "B" key to trigger the appearance of the sword formation model. When the robot enters the sword formation, the program will exit.

### 4.4 Model Interaction
By following the model operations described in section 3.2, when the model approaches and touches the ground parts, it will pick up the parts and equip them as part of the hierarchical modeling:

![Figure 5: Model Interaction Diagram](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Item_Interaction_1.png)

![Figure 5: Model Interaction Diagram](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Item_Interaction_2.png)

To trigger the program exit Easter Egg mentioned in section 3.3, press the "B" key until the sword formation model appears, then walk into the sword formation:

![Figure 5: Model Interaction Diagram](https://github.com/Starrywoof/Virtual-Scene-Modeling-of-Robot-with-OpenGL/blob/main/Pictures/Item_Interaction_3.png)


## 5. Prerequisites

- OpenGL library
- Compiler with C++ support

## 6. Usage

Contributions, bug reports, and feature requests are welcome! Please feel free to open an issue or submit a pull request.

