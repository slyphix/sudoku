
cc = g++ -std=c++14 -O3 -march=native -funroll-loops
mkdir = mkdir -p
rm = rm -f
build_dir = build

all : zdss helpers
opt : override zdss_flags += -DNOREPRINT -DNOVALIDATE
opt : zdss
helpers : zdsv spp

zdss : directory
	$(cc) $(zdss_flags) -o $(build_dir)/zdss zdss/zdss.cpp

zdsv : directory
	$(cc) $(zdsv_flags) -o $(build_dir)/zdsv zdsv/zdsv.cpp

spp : directory
	$(cc) -o $(build_dir)/spp spp/spp.cpp

formatverifier : directory
	$(cc) -DALLOWEMPTY -DALLOWMULT -o $(build_dir)/zdsfv zdsv/zdsv.cpp


.PHONY : directory clean
directory :
	$(mkdir) $(build_dir)

clean :
	$(rm) $(build_dir)/zdss $(build_dir)/spp $(build_dir)/zdsv $(build_dir)/zdsfv
