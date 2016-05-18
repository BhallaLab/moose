file(GLOB scheduling_SRC 
    "*.h"
    "*.cpp"
)

IF(NOTIFY_PROGRESS)
    ADD_DEFINITIONS("-DNOTIFY_PROGRESS")
ENDIF()

include_directories(../msg)
include_directories(../basecode)
add_library(scheduling ${scheduling_SRC})
