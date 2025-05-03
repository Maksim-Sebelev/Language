MAKE_DIR := make

FRONT_MAKE := $(MAKE_DIR)/make-front.mk
MIDLE_MAKE := $(MAKE_DIR)/make-midle.mk
BACK_MAKE  := $(MAKE_DIR)/make-back.mk



all:
	make make-front && make-midle && make-back


make-front:
	make -f $(FRONT_MAKE)

make-midle:
	make -f $(MIDLE_MAKE)

make-back:
	make -f $(BACK_MAKE)





