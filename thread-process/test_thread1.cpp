//
// Created by 姜倩云 on 2018/7/15.
//

/** 示例: 等待线程退出 **/
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
using namespace std;
void *thread_rotine(void *args)
{
    for (int i = 0; i < 10; ++i)
    {
        printf("B");
        fflush(stdout);
        usleep(20);
    }
    pthread_exit(NULL);
}

void *printout(void* args)
{
    int * p = (int *) args;
    while(true)
    {
        cout<<*p<<endl;
        //cout<<args<<endl;
        sleep(1);
    }
}

int main() {

    pthread_t thread[5];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&thread[i], NULL, printout, (void*) &i);
    }

    sleep(100);
    for (int i = 0; i < 5; i++) {
        pthread_cancel(thread[i]);
    }
}

//主线程退出以后，由这个主线程创建的子线程也随之消亡。
