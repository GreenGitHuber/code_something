#include <iostream>
#include <functional>
#include <memory>
#include <thread>
using namespace std;


class B 
{ 
    public: 
        void start() { 
            auto res = std::bind(&B::testfunc, *this); 
            //auto res = std::bind(&B::testfunc, this); //这里加不加 * 都可以 //执行testfunc ，方式1 ： res(); //执行testfunc ，方式2 ： 
            //auto res2 = std::make_shared<thread>(res); 
        } 

        void testfunc(void) { 
            cout << "wahahah" << endl; 
        } 
}; 

int main()
{ 
    B b; 
    b.start(); 
} 

//cankao : https://www.jianshu.com/p/7bcecdb07f5f