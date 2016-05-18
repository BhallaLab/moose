include_directories(../basecode ../msg  )

if(LIBSBML_FOUND)
    add_definitions(-DUSE_SBML)
endif()

add_library(shell 
	Shell.cpp	
	ShellCopy.cpp	
	ShellThreads.cpp	
	LoadModels.cpp 
	SaveModels.cpp 
	Neutral.cpp	
	Wildcard.cpp	
	testShell.cpp	
    )
