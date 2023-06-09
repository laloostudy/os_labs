#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <grp.h>
#include <sys/un.h>

using namespace std;


queue<string> querySet;

struct th_server_args
{
    char flag;
    int listen_socket;
    int conn_server;
    pthread_mutex_t mx;
};


void* server_send(void* args)
{
    char* flag = &static_cast<th_server_args*>(args)->flag;
    pthread_mutex_t mx = static_cast<th_server_args*>(args)->mx;
    int conn_server = static_cast<th_server_args*>(args)->conn_server;

    while((int)*flag != 10)
    {

        pthread_mutex_lock(&mx);
        if(!querySet.empty())
        {
            string message = "";
//            string message = querySet.front();
            querySet.pop();
            pthread_mutex_unlock(&mx);

            long host_id = gethostid();
            char host_id_str[sizeof(long) + 1];
            sprintf(host_id_str, "%ld", host_id);
            string response = message + ": " + host_id_str;

            vector<char> resp_c(response.begin(), response.end());
            int sendcount = send(conn_server, &resp_c[0], resp_c.size(),0);

            if (sendcount == -1)
            {
                perror("send error");
            }
            else
            {
                cout << "Send OK" << endl;
            }
        }
        else
        {
            pthread_mutex_unlock(&mx);
            sleep(1);
        }

    }
    pthread_exit((void*)1);
}


void* server_get(void* args)
{
    char* flag = &static_cast<th_server_args*>(args)->flag;
    int conn_server = static_cast<th_server_args*>(args)->conn_server;
    pthread_mutex_t mx = static_cast<th_server_args*>(args)->mx;

    const int RCV_SIZE = 1024;

    while((int)*flag != 10)
    {
        vector<char> buf(RCV_SIZE);
        string rcv;
        int reccount = recv(conn_server, &buf[0], RCV_SIZE, 0);

        if (reccount == -1)
        {
            perror("recv error");
            sleep(1);
        }
        else if (reccount == 0)
        {
            sleep(1);
        }
        else
        {
            rcv.append(buf.begin(), buf.end());
            cout << "Client request: " << rcv << endl;

            pthread_mutex_lock(&mx);
            querySet.push(rcv);
            pthread_mutex_unlock(&mx);
        }
    }
    pthread_exit((void*)1);
}


void* server_conn(void* args)
{
    char* flag = &static_cast<th_server_args*>(args)->flag;
    int* conn_server = &static_cast<th_server_args*>(args)->conn_server;
    int listen_socket = static_cast<th_server_args*>(args)->listen_socket;

    while((int)*flag != 10)
    {
        *conn_server = accept(listen_socket, (struct sockaddr*)NULL, NULL);

        if (*conn_server == -1)
        {
            perror("accept error");
            sleep(1);
        }
        else
        {
            cout << "\nClient connection" << endl;

            pthread_t th_server_send, th_server_get;
            pthread_create(&th_server_send, NULL, server_send, args);
            pthread_create(&th_server_get, NULL, server_get, args);

            void* status_1;
            void* status_2;
            int res_send = pthread_join(th_server_send, &status_1);
            int res_get = pthread_join(th_server_get, &status_1);
        }

        shutdown(*conn_server, 2);
        close(*conn_server);
    }
    pthread_exit((void*)1);
}


int main()
{
    th_server_args params;
    pthread_t th;

    struct sockaddr_un serverAddr{};
    int optval = 1;

    params.listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    fcntl(params.listen_socket, F_SETFL, O_NONBLOCK);

    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, "srsock.soc");

    bind(params.listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    setsockopt(params.listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    listen(params.listen_socket, SOMAXCONN);

    cout << "Server started\n" << endl;

    pthread_mutex_init(&params.mx, NULL);
    pthread_create(&th, NULL, server_conn, (void*)(&params));

    params.flag = getchar();

    void* status;
    int res = pthread_join(th, &status);

    cout << "\nServer finished" << endl;
    pthread_mutex_destroy(&params.mx);
    close(params.listen_socket);
    remove("srsock.soc");
    return 0;
}
