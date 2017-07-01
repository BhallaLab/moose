include_directories(../msg)
include_directories(../basecode)
add_library(device
    PulseGen.cpp
    DiffAmp.cpp
    PIDController.cpp
    RC.cpp
    )
