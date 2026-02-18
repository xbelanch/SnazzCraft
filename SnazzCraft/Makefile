CPPC = g++
CC = gcc
INCLUDES = "includes"
CPPFLAGS = -c -I$(INCLUDES)
CFLAGS = -c -I$(INCLUDES)
LDFLAGS = -lglfw -ldl -lGL -lnoise -lm #-static-libstdc++

all:
	make clean

	$(CC) $(CFLAGS) includes/glad.c -o build/glad.o

	$(CPPC) $(CPPFLAGS) src/main.cpp -o build/main.o

	$(CPPC) $(CPPFLAGS) src/engine/core/core.cpp -o build/core.o
	$(CPPC) $(CPPFLAGS) src/engine/core/mode.cpp -o build/mode.o
	$(CPPC) $(CPPFLAGS) src/engine/core/window.cpp -o build/window.o
	
	$(CPPC) $(CPPFLAGS) src/engine/input-handler/input-handler.cpp -o build/input-handler.o
	$(CPPC) $(CPPFLAGS) src/engine/input-handler/event.cpp -o build/event.o
	$(CPPC) $(CPPFLAGS) src/engine/input-handler/data.cpp -o build/data.o
	$(CPPC) $(CPPFLAGS) src/engine/input-handler/core-callback.cpp -o build/core-callback.o

	$(CPPC) $(CPPFLAGS) src/engine/gui/world-gui.cpp -o build/world-gui.o
	$(CPPC) $(CPPFLAGS) src/engine/gui/widget-collection.cpp -o build/widget-collection.o
	$(CPPC) $(CPPFLAGS) src/engine/gui/panel.cpp -o build/panel.o
	$(CPPC) $(CPPFLAGS) src/engine/gui/main-menu-gui.cpp -o build/main-menu-gui.o
	$(CPPC) $(CPPFLAGS) src/engine/gui/gui.cpp -o build/gui.o
	$(CPPC) $(CPPFLAGS) src/engine/gui/button.cpp -o build/button.o

	$(CPPC) $(CPPFLAGS) src/engine/entity/entity.cpp -o build/entity.o
	$(CPPC) $(CPPFLAGS) src/engine/entity/user.cpp -o build/user.o

	$(CPPC) $(CPPFLAGS) src/engine/fps-tracker/fps-tracker.cpp -o build/fps-tracker.o

	$(CPPC) $(CPPFLAGS) src/engine/height-map/noise-module.cpp -o build/noise-module.o
	$(CPPC) $(CPPFLAGS) src/engine/height-map/height-map.cpp -o build/height-map.o

	$(CPPC) $(CPPFLAGS) src/engine/hitbox/hitbox.cpp -o build/hitbox.o

	$(CPPC) $(CPPFLAGS) src/engine/mesh/mesh.cpp -o build/mesh.o

	$(CPPC) $(CPPFLAGS) src/engine/texture/texture.cpp -o build/texture.o
	$(CPPC) $(CPPFLAGS) src/engine/texture/solid-colors.cpp -o build/solid-colors.o
	$(CPPC) $(CPPFLAGS) src/engine/texture/atlas.cpp -o build/atlas.o

	$(CPPC) $(CPPFLAGS) src/engine/utilities/math.cpp -o build/math.o
	$(CPPC) $(CPPFLAGS) src/engine/utilities/text-file-handling.cpp -o build/text-file-handling.o

	$(CPPC) $(CPPFLAGS) src/engine/vertice/vertice-3d.cpp -o build/vertice-3d.o

	$(CPPC) $(CPPFLAGS) src/engine/world/world.cpp -o build/world.o
	$(CPPC) $(CPPFLAGS) src/engine/world/voxel.cpp -o build/voxel.o
	$(CPPC) $(CPPFLAGS) src/engine/world/chunk.cpp -o build/chunk.o

	$(CPPC) build/*.o -o bin/executable $(LDFLAGS)

clean:
	rm -f build/*.o bin/executable

