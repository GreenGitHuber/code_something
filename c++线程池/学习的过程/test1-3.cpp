
// 这一部分可以 搜一下 c++11线程 的创建和使用的方法

#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <condition_variable>
using namespace std;

class Test{ 
private:
	list<shared_ptr<thread>> thread_list;
public:
	void test_ptr_thread() {
		for (int i = 0; i < 3; i++) {
			auto res = bind(&Test::threadWork, this);
			auto res2 = make_shared<thread>(res);
			thread_list.push_back(res2);
		}
	}

	void threadWork(void) {
		std::cout << "threadWork" << std::endl;
	}

	void stop() {
		for (auto t : thread_list) {
			cout << "stop " << endl;
			t->join();
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