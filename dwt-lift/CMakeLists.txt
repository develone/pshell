add_library(dwt-lift INTERFACE)
target_include_directories(lifting INTERFACE ${CMAKE_CURRENT_LIST_DIR}/dwt-lift-lib)
target_sources(lifting INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/dwt-lift-lib/dwt-lift.c
 
	${CMAKE_CURRENT_LIST_DIR}/dwt-lift-lib/dwt-lift.h
 
	

)


