all: AI

.PHONY : AI
AI: libs
	cd AI && \
	wiimake ../melee.iso AI.ini && \
	cd ..

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