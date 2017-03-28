# panorama-360-test
- a panorama stitch program build based on surround 360
- can work in 32-bit system
- simplify the algorithm in "surround 360"
- to be continue..
## run methods
- have installed gcc cflags pkg-config opencv2.4.11 on your linux system
- open your terminal,input below 
- g++ -c pano_util.cpp
- g++ -c testOpticalView.cpp
- g++ -o test testCreatPano.cpp pano_util.o testOpticalView.o  `pkg-config opencv --cflags --libs`
- ./test <top_1.png> <bottom_1.png> <bottom_2.png> <side.png> <result.png>
- in the end ,the result will be stored in the result.png ,you can view the panorama by opening it in some specific devices.such as samsung gear,oculus vr...
