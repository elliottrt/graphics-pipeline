
running on MacOS/Linux
	make sure SDL3 and tiffio are installed systemwide or somewhere CMake looks for them
	make sure CMake and Make are installed
	execute the following commands:
		```
		mkdir build
		cd build
		cmake ..
		make
		./graphics-pipeline
		```

running on Windows (don't know if this works)
	open the folder in Visual Studio
	Visual Studio will detect CMake and build it for you
	Use Visual Studio to run the program
