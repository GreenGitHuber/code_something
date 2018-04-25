#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  //for htonl() and htons()
#include <fcntl.h>
#include <sys/epoll.h>
#include <list>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <iomanip> //for std::setw()/setfill()
#include <unistd.h>

using namespace std;


int create_and_bind(char* ip ="0.0.0.0",int port = 8889){

    int m_listenfd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listenfd == -1){
		return false;
    }

    //修改time_wait等待的时间
    int on = 1;
	::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

    struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);

	if (bind(m_listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        return false;
    }

	if (::listen(m_listenfd, 50) == -1){
        return false;
    }

    return m_listenfd;
}



int main(){
    int m_listenfd = create_and_bind();

    //epoll
    int m_epollfd = epoll_create(1);
	if (m_epollfd == -1)
		return false;

	struct epoll_event e;
	memset(&e, 0, sizeof(e));
	e.events = EPOLLIN | EPOLLRDHUP;
	e.data.fd = m_listenfd;
	
    if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &e) == -1)
	{return false;}

    while(1){
        struct epoll_event event[1024];
		int n = ::epoll_wait(m_epollfd, event, 1024, 10);

        for(int i=0;i<n;i++){
            if((event[i].events & EPOLLERR) || (event[i].events & EPOLLHUP) || (!(event[i].events & EPOLLIN))) {
                cout<<"error"<<endl;
                return -1;
            }
            else if (m_listenfd == event[i].data.fd){
                // 监听到新的fd
                cout<<"监听到新的fd"<<endl;


            }
            else{
                //read/write
                cout<<"read/write"<<endl;

            }
        }
    }

    close(m_listenfd);
    return 0;
}