Masterrain: Masterrain.cc
	g++ -std=gnu++11 Masterrain.cc -I/usr/include/SDL2 -I/usr/include/cairo -I/usr/local/include/chipmunk -L/mingw32/lib -lSDL2main -lSDL2 -lcairo -lchipmunk -lpthread -o Masterrain
