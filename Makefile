all : memwatch
memwatch : memwatch.cpp 
	g++ -O3 -Wall -Wextra -Wpedantic -Werror -std=c++17 -o memwatch memwatch.cpp 
clean :
	rm memwatch
