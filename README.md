# SurfelPlyViewer
![demo](https://github.com/ShunChengWu/SurfelPlyViewer/blob/main/demo.png)

# Ply file format 
The target ply file should save surfel properties to:
```
property float x
property float y
property float z
property ushort label
property float nx
property float ny
property float nz
property uchar red
property uchar green
property uchar blue
[optional] property float radius
```
RGB and normal values are optional. 

# Dependencies 
Eigen3

# Build
```
mkdir build; cd build; cmake ..; make;
```

# Control 
Mouse left button: rotation  
Mouse right button: height adjustment   
Mouse scroll: distance to the object  
Mouse scroll button: translation  

# hotkey
Esc: exit  
c: show grid  

