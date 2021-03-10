# Minecraft Renderer Attempt Number 2

This is a minecraft renderer using opengl 4.6. Super early development. It is starting to work. Main issue rn are texture UVs and the handling of tint overlays. Z fighting in distant grass. Just fixed block rotation and grass biome color calculation. Taking a break and working on documentation. https://noahwhygodwhy.github.io/MCR2/ Accessing certain pages doesn't work as doxygen likes to put underscores at the front of some html file names. WIP :\

It is working a lot better than attempt #1 though. Chunk creation has not only been significantly optimized, but has also been threaded, so loading chunks is almost as seamless as real minecraft. Block rotation is also much more efficient. Memory usage has been cut by something like 80%. I was being kinda stupid before lol. Once I get UVs done right, i'll start working on other things like water, or maybe some basic entities like chests.

![Preview](https://i.imgur.com/eWAIbED.jpeg)

I'll make it user friendly in the future, but if you reaaaallly want to try it out I'm sure you can figure it out. main.cpp has the path to the save file. The paths to the minecraft asset stuff is all relative. There's a copy of it in the repository. It needs nuget packages glm and glfw in visual studio 19. Language standard C++17. Comment out "#define TEST" for better performance (enables threading). Also have to have command line arguments to disable depreciation warnings as one of the header files it uses had a bunch of depreciated stuff. Will update when it's working better.

Application flow is as follows:
1. A "Renderer" is created and it initializes the opengl context.
2. The Renderer is used to load all textures.
3. An "Asset" is created, parses all model/blockstate jsons, and is given the mapping of texture names to texture indicies.
4. A "World" is created, and begins loading chunks, does so with threads to not hold up main thread.
5. The Renderer draw loop starts.
