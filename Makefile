
cc = g++
cc_flags = -std=c++14 -O3 -march=native -funroll-loops
cc_call = $(cc) $(cc_flags)
mkdir = mkdir -p
rm = rm -f
build_dir = build

ifdef opt
zdss_flags += -DNOREPRINT -DNOVALIDATE
endif

all : zdss helpers
helpers : zdsv spp

zdss : directory
	$(cc_call) $(zdss_flags) -o $(build_dir)/zdss zdss/zdss.cpp

zdsv : directory
	$(cc_call) $(zdsv_flags) -o $(build_dir)/zdsv zdsv/zdsv.cpp

spp : directory
	$(cc_call) -o $(build_dir)/spp spp/spp.cpp

formatverifier : directory
	$(cc_call) -DALLOWEMPTY -DALLOWMULT -o $(build_dir)/zdsfv zdsv/zdsv.cpp


.PHONY : directory clean
directory :
	$(mkdir) $(build_dir)

clean :
	$(rm) $(build_dir)/zdss $(build_dir)/spp $(build_dir)/zdsv $(build_dir)/zdsfv
