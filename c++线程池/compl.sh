
g++ -c thread_pool.cpp -std=c++11
g++ main.cpp -o main thread_pool.o -std=c++11 -lpthread