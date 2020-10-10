# SurfelPlyViewer
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
property float quality
property float radius
```
RGB and normal values are optional. 

# Dependencies 
Eigen3

# Build
```
mkdir build; cd build; cmake ..; make;
```
