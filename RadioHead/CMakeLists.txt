add_library(radiohead INTERFACE)
target_include_directories(radiohead INTERFACE ${CMAKE_CURRENT_LIST_DIR}/radiohead-lib)
target_sources(radiohead INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/radiohead-lib/cstdint.h
    ${CMAKE_CURRENT_LIST_DIR}/radiohead-lib/RHCRC.cpp
    ${CMAKE_CURRENT_LIST_DIR}/radiohead-lib/RadioHead.h
    ${CMAKE_CURRENT_LIST_DIR}/radiohead-lib/RHCRC.h
    
 
	

)
