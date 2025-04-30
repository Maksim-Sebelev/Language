THIS_FILE 		  := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_MAKEFILE_WAY := $(dir $(THIS_FILE))


.PHONY: all run rerun clean rebuild

FRONT_MAKE := FrontEnd/Src/Makefile
SET_MAIN_MAKE_FOR_OTHER_MAKES = MAIN_MAKEFILE_WAY=$(THIS_MAKEFILE_WAY)


all:
	make -f $(FRONT_MAKE) $(SET_MAIN_MAKE_FOR_OTHER_MAKES)

run:
	make -f $(FRONT_MAKE) $(SET_MAIN_MAKE_FOR_OTHER_MAKES) run


clean:
	make -f $(FRONT_MAKE) $(SET_MAIN_MAKE_FOR_OTHER_MAKES) clean

rerun:
	make && make run

rebuild:
	make clean && make


git:
	git add --all
	git commit -m "ZoV"
	git push --all