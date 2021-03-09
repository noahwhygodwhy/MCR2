# Mineccraft Renderer Attempt Number 2

This is a minecraft renderer using opengl 4.6. Super early development. It is starting to work. Main issue rn are texture UVs and the handling of tint overlays. Z fighting in distant grass. Just fixed block rotation and grass biome color calculation. Taking a break and working on documentation. https://noahwhygodwhy.github.io/MCR2/ Accessing certain pages doesn't work as doxygen likes to put underscores at the front of some html file names. WIP :\

If you reaaaallly want to try it out I'm sure you can figure it out. main.cpp has the absolute path to the save file. The asset stuff is all relative. It needs nuget packages glm and glfw in visual studio 19. Language standard C++17. Comment out "#define TEST" for better performance (enables threading). Also have to have command line arguments to disable depreciation warnings as one of the header files it uses had a bunch of depreciated stuff. Will update when it's working better.



