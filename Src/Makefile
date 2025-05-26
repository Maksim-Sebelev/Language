MAKE_DIR := make

FRONT_MAKE := $(MAKE_DIR)/make-front.mk
MIDLE_MAKE := $(MAKE_DIR)/make-midle.mk
BACK_MAKE  := $(MAKE_DIR)/make-back.mk



all:
	make front
	make midle
	make back


front:
	make -f $(FRONT_MAKE)

midle:
	make -f $(MIDLE_MAKE)

back:
	make -f $(BACK_MAKE)

run:
	make -f $(FRONT_MAKE) run
	make -f $(MIDLE_MAKE) run
	make -f $(BACK_MAKE)  run

clean:
	rm -rf bin/
	rm -rf build/