//c++11 下使用多线程

#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <condition_variable>
#include <Windows.h>  
using namespace std;

void thread01()
{
	for (int i = 0; i < 5; i++)
	{
		cout << "Thread 01 is working ！" << endl;
		Sleep(100);
	}
}
void thread02()
{
	for (int i = 0; i < 5; i++)
	{
		cout << "Thread 02 is working ！" << endl;
		Sleep(200);
	}
}

int main()
{
	//step1 :
	//thread task01(thread01);
	//thread task02(thread02);
	//task01.join();
	//task02.join();

	//for (int i = 0; i < 5; i++)
	//{
	//	cout << "Main thread is working ！" << endl;
	//	Sleep(200);
	//}


	//step2 :
	vector<thread * > thread_list;

	thread th1(thread01);
	thread th2(thread02);

	thread_list.push_back(&th1);
	thread_list.push_back(&th2);

	for (auto it = thread_list.begin(); it != thread_list.end(); it++) {
		(*it)->join();
	}

    system("pause");
    return 0;
}