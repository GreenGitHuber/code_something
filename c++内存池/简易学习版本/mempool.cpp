
#include <iostream>

using namespace std;

template<int ObjectSize, int NumofObjects = 20>   
class MemPool {
private:
	struct FreeNode {
		FreeNode* pNext;
		char data[ObjectSize];
	};

	FreeNode * freeNodeHeader;     //表示空闲链表
public:
	MemPool() {
		//init freeNodeHeader
		this->freeNodeHeader = new FreeNode[NumofObjects];
		for (int i = 0; i < NumofObjects - 1; i++) {
			freeNodeHeader[i].pNext = &freeNodeHeader[i + 1];
		}
		freeNodeHeader[NumofObjects-1].pNext = NULL;

	}

	~MemPool() {
		cout << " ~ 析构函数 " << endl;
		delete[] freeNodeHeader;
	}

	//函数的实现在下面
	void* malloc() {
		if (freeNodeHeader==NULL) {  //空间没有了
			//分配一段新的空间
			cout << "重新添加了空间" << endl;
			FreeNode * newfreeNodeHeader = new FreeNode[NumofObjects];
			for (int i = 0; i < NumofObjects - 1; i++) {
				newfreeNodeHeader[i].pNext = &newfreeNodeHeader[i + 1];
			}
			newfreeNodeHeader[NumofObjects-1].pNext = NULL;   
			
			//于是又有了空间
			freeNodeHeader = newfreeNodeHeader;
		}
		cout << "分配一个空间" << endl;
		FreeNode * ret = freeNodeHeader;
		freeNodeHeader = freeNodeHeader->pNext;
		ret->pNext = NULL;
		return ret;
	}

	void free(void* p) {
		cout << "释放一个空间" << endl;
		FreeNode* pNode = (FreeNode*)p;
		pNode->pNext = freeNodeHeader;  //将释放的节点插入空闲节点头部
		freeNodeHeader = pNode;
	}
};

//一个实例类，用来作为测试
class ActualClass {
	static int count;
	int No;

public:
	ActualClass() {
		No = count;
		count++;
	}

	void print() {
		cout << this << ": ";
		cout << "the " << No << " object" << endl;
	}

	void* operator new(size_t size);
	void operator delete(void* p);


	void* operator new[](size_t size);
	void operator delete[](void* p);
};

MemPool<4, 5> mp;

void* ActualClass::operator new(size_t size) {
	cout << "size " << size << endl;
	return mp.malloc();
}

void ActualClass::operator delete(void* p) {
	mp.free(p);
}

void* ActualClass::operator new[](size_t size) {
	cout << "size " << size << endl;
	cout << "自定义的内存池并不支持申请多个空间,所以调用默认的malloc" << endl;
	return malloc(size);
}

void ActualClass::operator delete[](void* p) {
	cout << "调用默认的free" << endl;
	free(p);
}

int ActualClass::count = 0;



int main()
{
	for (int i = 0; i < 3; i++) {
		ActualClass* p = new ActualClass;
		p->print();
	}

	ActualClass* p1 = NULL;
	p1 = new ActualClass;
	p1->print();

	ActualClass* p2 = new ActualClass;
	p2->print();

	delete(p1);

	ActualClass* p3 = new ActualClass;
	p3->print();

	//to-do
	//还不能分配连续的空间
	//ActualClass* p4 = new ActualClass[5];
	//ActualClass* p5 = new ActualClass[5];
	//ActualClass* p6 = new ActualClass[5];

	system("pause");
	return 0;
};


