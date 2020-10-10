# SurfelPlyViewer
![RGB](main/img/rgb.png)
![Normal](main/img/normal.png)
![Label](main/img/label.png)

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

# Control 
Mouse left button: rotation  
Mouse right button: height adjustment   
Mouse scroll: distance to the object  
Mouse scroll button: translation  

# hotkey
Esc: exit  
c: show grid  

