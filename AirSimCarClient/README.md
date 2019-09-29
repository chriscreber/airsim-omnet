Based on the AirSim HelloCar C++ example.

## Build Instructions

```
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
```

## Run Instructions

```
Add as many cars to settings.json as you want. Follow name pattern: Car4001 Car4002 ... CarN
- Press the play button in Unreal

In the directory multiCarNetworkSim edit the omnetpp.ini file to have a matching number of clients.
- Press play button in omnetpp
- Press F6 to run without animation

$ bash runCars
- Enter matching number of cars.
- This runs the CarAuto binary. To run the CarMan file you have to do it manually
```
