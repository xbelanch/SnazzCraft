CPPC = g++
CC = gcc
CPPFLAGS = -c
CFLAGS = -c
LDFLAGS = -lglfw -ldl -lGL -lnoise -lm #-static-libstdc++

all:
	make clean

	$(CC) $(CFLAGS) includes/glad.c -o build/glad.o

	$(CPPC) $(CPPFLAGS) src/main.cpp -o build/main.o
	$(CPPC) $(CPPFLAGS) src/engine/core/core.cpp -o build/core.o
	$(CPPC) $(CPPFLAGS) src/engine/mesh/mesh.cpp -o build/mesh.o
	$(CPPC) $(CPPFLAGS) src/engine/texture/texture.cpp -o build/texture.o
	$(CPPC) $(CPPFLAGS) src/engine/texture/atlas/atlas.cpp -o build/atlas.o
	$(CPPC) $(CPPFLAGS) src/engine/entity/entity.cpp -o build/entity.o
	$(CPPC) $(CPPFLAGS) src/engine/world/world.cpp -o build/world.o
	$(CPPC) $(CPPFLAGS) src/engine/world/chunk/chunk.cpp -o build/chunk.o
	$(CPPC) $(CPPFLAGS) src/engine/world/voxel/voxel.cpp -o build/voxel.o
	$(CPPC) $(CPPFLAGS) src/engine/perlin-noise/noise-module.cpp -o build/perlin-noise.o 
	$(CPPC) $(CPPFLAGS) src/engine/height-map/height-map.cpp -o build/height-map.o
	$(CPPC) $(CPPFLAGS) src/engine/utilities/file-handling/text-file-handling/text-file-handling.cpp -o build/text-file-handling.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/button/button.cpp -o build/button.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/gui.cpp -o build/gui.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/panel/panel.cpp -o build/panel.o
	$(CPPC) $(CPPFLAGS) src/engine/core/input-handler/event/event.cpp -o build/event.o
	$(CPPC) $(CPPFLAGS) src/engine/core/input-handler/input-handler.cpp -o build/input-handler.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/widget-collection/widget-collection.cpp -o build/widget-collection.o
	$(CPPC) $(CPPFLAGS) src/engine/core/mode/mode.cpp -o build/mode.o
	$(CPPC) $(CPPFLAGS) src/engine/entity/user/user.cpp -o build/user.o
	$(CPPC) $(CPPFLAGS) src/engine/core/input-handler/event/data/data.cpp -o build/data.o
	$(CPPC) $(CPPFLAGS) src/engine/core/input-handler/core-callback/core-callback.cpp -o build/callback.o
	$(CPPC) $(CPPFLAGS) src/engine/vertice/vertice-3d/vertice-3d.cpp -o build/vertice-3d.o
	$(CPPC) $(CPPFLAGS) src/engine/fps-tracker/fps-tracker.cpp -o build/fps-tracker.o
	$(CPPC) $(CPPFLAGS) src/engine/utilities/math.cpp -o build/math.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/main-menu-gui/main-menu-gui.cpp -o build/main-menu-gui.o
	$(CPPC) $(CPPFLAGS) src/engine/core/gui/world-gui/world-gui.cpp -o build/world-gui.o
	$(CPPC) $(CPPFLAGS) src/engine/core/window/window.cpp -o build/window.o
	$(CPPC) $(CPPFLAGS) src/engine/hitbox/hitbox.cpp -o build/hitbox.o

	$(CPPC) build/*.o -o bin/executable $(LDFLAGS)

clean:
	rm -f build/*.o bin/executable
