all: AI

.PHONY : AI
AI: libs includes
	cd AI && \
	wiimake ../melee.iso AI.ini && \
	cd ..

.PHONY : includes
includes: 
	mkdir -p include/mml && \
	cp MeleeModdingLibrary/src/*.h include/mml 

.PHONY : libs
libs:
	cd MeleeModdingLibrary && \
	make libs && \
	mkdir -p ../libs && \
	mv libmml* ../libs && \
	cd ..

.PHONY : clean
clean:
	rm -rf libs && \
	cd MeleeModdingLibrary && \
	make clean && \
	cd ..