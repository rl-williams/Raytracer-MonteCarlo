# Raytracer-MonteCarlo

## description
Rendering system for physically realistic 3D images, created in Qt Creator using C++ with OpenGL. Created for Computer Graphics with Dr. Norm Badler at the University of Pennsylvania. Individual project over three months (finished December 2014).

## features
- Direct and Monte Carlo Indirect Illumination (renders using each one or both)
- OBJ loader and scene graph (GUI created with Qt Creator and OpenGL)
- Ray-primitive intersection: cube, sphere, cylinder, and mesh (triangular plane) intersection functionality
- Point lights and/or area lights with soft shadows
- Reflective, translucent/refractive, specular, and diffuse materials (each of whose attributes may be adjusted in the configuration file)
- Parallel processing techniques (multi-threading with OpenMP) used to improve performance
- 
- anti-aliasing
- caustics
- color bleeding
- Takes keyboard input to rotate/translate the camera and to output a .BMP file of the raytraced scene

## results
- ![alt tag](https://github.com/rl-williams/Raytracer-MonteCarlo/blob/master/SceneGraph/final_images/mc-indirect-100samples.bmp)
- ![alt tag](https://github.com/rl-williams/Raytracer-MonteCarlo/blob/master/SceneGraph/final_images/mc-animals.png)

