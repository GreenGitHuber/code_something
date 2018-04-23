// 绕过了bind 函数，通过thread api 直接想线程要执行的函数绑定上去。我更喜欢这种写法。

#include <iostream>
#include <functional>
#include <memory>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <condition_variable>
using namespace std;



class Test {
private:
	vector<std::thread *> vector_threads;
public:
	void threadWork() {
		std::cout << "threadWork" << std::endl;
		Sleep(300);  //300ms
	}

	void stop() {
		for (auto it = vector_threads.begin(); it != vector_threads.end(); it++) {
			(*it)->join();
		}
	}

	void test_ptr_thread() {
		for (int i = 0; i < 3; i++) {
            // 绕过了bind 函数，通过thread api 直接想线程要执行的函数绑定上去。我更喜欢这种写法。
			thread * th = new thread(&Test::threadWork,this);   //注意：不能复制 std :: thread 类型的对象。 thread t(&Test::threadWork,this) vector_threads.push_back(t)  // 这样是会报错的！！
			vector_threads.push_back(th);
		}
	}
};

int main()
{
	
	Test t;
	t.test_ptr_thread();
	t.stop();

	system("pause");
	return 0;
}
