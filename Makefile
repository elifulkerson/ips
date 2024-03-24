.PHONY: all
all: ips xips

ips: ips.cpp
	g++ -std=c++11 -o ips ips.cpp

xips: xips.cpp
	g++ -std=c++11 -o xips xips.cpp

clean:
	rm ips.exe

test:
	python maketests.py > tests.sh && sh ./tests.sh


