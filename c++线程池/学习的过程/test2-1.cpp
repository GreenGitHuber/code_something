// 这个是可以在window下面的ide里面运行的
// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h" //这个不能删除啊
// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//


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


class ThreadPool {
private:
	int threadNum;
	list< function<void(void)> > Task_list;    //任务队列
	mutex m_;
	vector<shared_ptr<thread> > threads;       //线程池
	bool isRuning;
	std::condition_variable_any condition_empty_;

public:
	ThreadPool(int Num) {
		this->threadNum = Num;
		this->isRuning = false;
	}
	~ThreadPool() {
		if (isRuning) {
			stop();
		}
	}


	void start() {
		if (!isRuning) {
			this->isRuning = true;

			for (int i = 0; i < threadNum; i++) {
				auto res = bind(&ThreadPool::threadWork, this);
				auto res2 = make_shared<thread>(res);
				threads.push_back(res2);
			}
			cout << "ThreadPool is running " << endl;
			
			return;
		}
	}

	void threadWork() {
		cout << "threadWork" << endl;
		function<void(void)>  task = NULL;
		while (isRuning) {
			{
				std::lock_guard<std::mutex> guard(this->m_);    //类 lock_guard 是互斥封装器 ， 锁离开作用域时自动释放

				if (Task_list.empty()) {
					condition_empty_.wait(this->m_);  //等待有任务到来被唤醒
				}
				if(!Task_list.empty())
				{
					task = Task_list.front();  //从任务队列中获取最开始任务   // typedef std::function<void(void)> Task;
					Task_list.pop_front();     //将取走的任务弹出任务队列
				}
				else {
					continue;
				}
				task();  //执行任务函数
			} //释放锁
		}
	}

	void append(function<void(void)> task) {
		std::lock_guard<std::mutex> guard(this->m_);
		this->Task_list.push_front(task);				//将该任务加入任务队列
		condition_empty_.notify_one();					//唤醒某个线程来执行此任务
		return ;
	}

	void stop() {
		if (isRuning)
		{
			isRuning = false;
			for (auto t : threads)
			{
				cout << "终止" << endl;
				t->join();  //循环等待线程终止
			}
		}
	}
};


void fun(void)
{
	std::cout << "hello" << std::endl;
}

int main()
{
	{

		ThreadPool pool(5);
		pool.start();

		int tasknum = 10;
		while (tasknum--) {
			pool.append(fun);
		}

	}

	system("pause");
	return 0;
}
