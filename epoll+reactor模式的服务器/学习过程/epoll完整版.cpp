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
#include <string>

using namespace std;


int create_and_bind(char* ip ="0.0.0.0",int port = 8889){

    int m_listenfd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listenfd == -1){
		return false;
    }

    //修改time_wait等待的时间
    int on = 1;
	setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

    struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);

	if (bind(m_listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        return false;
    }

	if (listen(m_listenfd, 50) == -1){
        return false;
    }

    return m_listenfd;
}


bool close_client(int m_epollfd,int clientfd)
{
	if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, clientfd, NULL) == -1)
	{
		std::cout << "close client socket failed as call epoll_ctl failed" << std::endl;
	}
	close(clientfd);
	return true;
}

int main(){
    int m_listenfd = create_and_bind();

    //epoll_create
    int m_epollfd = epoll_create(1);
	if (m_epollfd == -1)
		return false;

    //epoll_ctl epoll 添加监听的事件
	struct epoll_event e;
	memset(&e, 0, sizeof(e));
	e.events = EPOLLIN | EPOLLRDHUP;
	e.data.fd = m_listenfd;
    if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &e) == -1)
	{return false;}

    //epoll_wait
    while(1){
        struct epoll_event event[1024];
		int n = ::epoll_wait(m_epollfd, event, 1024, 10);

        for(int i=0;i<n;i++){
            if((event[i].events & EPOLLERR) || (event[i].events & EPOLLHUP) || (!(event[i].events & EPOLLIN))) {
                cout<<"error"<<endl;
                return -1;
            }
            //如果监听到的事件是　m_listenfd，那表示的是监听到新的fd
            else if (m_listenfd == event[i].data.fd){
                // 监听到新的fd
                cout<<"监听到新的fd"<<endl;

                int newfd;
		        struct sockaddr_in clientaddr;
		        socklen_t addrlen;
                newfd = accept(m_listenfd, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1){
                    cout<<"accept error"<<endl;
                    return -1;
                }

                std::cout << "new client connected: " << ::inet_ntoa(clientaddr.sin_addr) << ":" << ::ntohs(clientaddr.sin_port) << std::endl;

                //将新socket设置为non-blocking
                int oldflag = ::fcntl(newfd, F_GETFL, 0);
                int newflag = oldflag | O_NONBLOCK;
                if (fcntl(newfd, F_SETFL, newflag) == -1)
                {
                    std::cout << "fcntl error, oldflag =" << oldflag << ", newflag = " << newflag << std::endl;
                    continue;
                }

                //将监听到的fd　注册到epoll的监听事件里面
                struct epoll_event e;
                memset(&e, 0, sizeof(e));
                e.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                e.data.fd = newfd;

                if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, newfd, &e) == -1)
                {
                    std::cout << "epoll_ctl error, fd =" << newfd << std::endl;
                }

            }
            else{
                //read and write
                cout<<"read/write"<<endl;
                string strclientmsg;
                char buf[512];  

                //read
                while(1){
                    memset(buf, 0, sizeof(buf));
                    int count = read (event[i].data.fd, buf, sizeof(buf));  
                    cout<<"--debug msg -- count : "<<count<<endl;
                    //这里要注意一下，收到一个消息之后count是多大?
                    if(count==-1){
            			if (errno == EWOULDBLOCK){  
                            //例如，以 O_NONBLOCK的标志打开文件/socket/FIFO，如果你连续做read操作而没有数据可读。
                            //此时程序不会阻塞起来等待数据准备就绪返回，read函数会返回一个错误EAGAIN，提示你的应用程序现在没有数据可读请稍后再试。
					        break;
                        }
                        cout << "recv error, client disconnected"<<endl;
                        break;
                    }
                    else if(count==0){
                        cout << "peer closed, client disconnected"<<endl;
                        break;
                    }
                    strclientmsg += buf;
                }//end while
                std::cout << "client msg: " << strclientmsg;

                //write
                while (true)
                {
                    strclientmsg="server reply : "+strclientmsg;
                    int nSent = send(event[i].data.fd, strclientmsg.c_str(), strclientmsg.length(), 0);
                    cout<<"--debug msg -- nSent : "<<nSent<<endl;
                    if (nSent == -1)
                    {
                        if (errno == EWOULDBLOCK)
                        {
                            continue;
                        }
                        else
                        {
                            std::cout << "send error "<< std::endl;
                            close_client(m_epollfd,event[i].data.fd);
                            break;
                        }
                    }

                    std::cout << "send: " << strclientmsg;
                    strclientmsg.erase(0, nSent);

                    if (strclientmsg.empty())
                        break;
                }
            }
        }
    }

    close(m_epollfd);
    close(m_listenfd);
    return 0;
}