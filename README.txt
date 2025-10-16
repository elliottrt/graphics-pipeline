
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

camera movement:
	w - move forwards
	s - move backwards
	a - move left
	d - move right
	space - move up
	shift - move down

	up arrow - look up
	down arrow - look down
	left arrow - look left
	right arrow - look right

	+ - zoom in
	- - zoom out
	
	g (holding) - move globally instead of locally

gui:
	the window in the bottom left has the gui.
	it has options for:
		resetting camera to origin
		setting the texture tiling mode
		setting the texture sample mode

extra credit:
	bilinear sampling, enabled in the gui
	animated sprite
