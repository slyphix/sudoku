
command = g++ -std=c++14 -O3 -march=native -funroll-loops
dir = .

basic : zdss spp zdsv

zdss :
	$(command) $(zdss_opts) -o $(dir)/zdss zdss/zdss.cpp

zdssoptimized :
	$(command) -DNOREPRINT -DNOVALIDATE -o $(dir)/zdss zdss/zdss.cpp

spp :
	$(command) -o $(dir)/spp spp/spp.cpp

zdsv :
	$(command) -o $(dir)/zdsv zdsv/zdsv.cpp

formatcheck :
	$(command) -DALLOWEMPTY -DALLOWMULT -o $(dir)/zdsfv zdsv/zdsv.cpp

.PHONY : clean
clean :
	rm -f $(dir)/zdss $(dir)/spp $(dir)/zdsv $(dir)/zdsfv
