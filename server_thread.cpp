#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#include <pthread.h>

#define DEFAULT_PORT (5000)

using namespace std;

static int listener; //fd
static volatile unsigned long process_count;

static int setup_server_socket(int port, bool block=false)
{
    int sock;
    struct sockaddr_in sin;
    int yes=1;

    // 以 SOCK_STREAM(TCP) 方式获取 socket，失败则 log 并且退出
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }
    // 设置套接字选项
    // 要设置的套接字 / 所在协议层（指定协议）/ 有效避免 time_wait / yes=1 代表开启
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    // 将 sin 全部初始化，避免出现问题
    memset(&sin, 0, sizeof sin);

    // 指定协议簇
    sin.sin_family = AF_INET;
    // 指定 ip(0.0.0.0) host 转 网络字节序
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    // 指定端口号
    sin.sin_port = htons(port);

    // socket 绑定网络地址
    if (bind(sock, (struct sockaddr *) &sin, sizeof sin) < 0) {
        close(sock);
        perror("bind");
        exit(-1);
    }

    // 监听该 socket，1024 是待连接队列的大小
    if (listen(sock, 1024) < 0) {
        // 监听失败，关闭 socket 并且退出
        close(sock);
        perror("listen");
        exit(-1);
    }

    return sock;
}

// 工作线程
void* worker(void *data)
{
    // 接收数据的 buffer
    char *buf = (char*)malloc(128);
    // malloc 分配失败，直接 return
    if (!buf) {
        fprintf(stderr, "worker: no memory.\n");
        return NULL;
    }

    // 死循环一直工作
    for (;;) {
        restart:
        // 客户端地址
        struct sockaddr_in client_addr;
        // 获取客户端地址长度(确认是 ipv4 还是 ipv6)
        socklen_t client_addr_len = sizeof client_addr;
        // accept 从监听的队列获取 socket，如果没有则阻塞
        int client = accept(listener, (struct sockaddr*)&client_addr, &client_addr_len);
        // 调用失败直接下次循环
        if (client < 0) {
            perror("accept");
            continue;
        }

        // 调用成功
        for (;;) {
            // 从 client 这个 socket 中获取内容并写入 buf
            // n 是有效的数据长度，应当 <= 128
            int n = ::read(client, buf, 128);
            // n > 0 则有数据
            if (n > 0) {
                int m = 0;
                while (m < n) {
                    // 往 client 中写数据，从 buf+m 开始，写 n-m 的长度
                    // 但是不一定能一次写完，写了多少就是 o 的值
                    // 倒计时满或者写满则触发发送
                    int o = ::write(client, buf+m, n-m);
                    // 如果出现问题，则关闭 client 重启
                    if (o < 0) {
                        perror("write");
                        close(client);
                        goto restart;
                    }
                    m += o;
                }
                // 线程安全的计数，先 get 再 ++
                // process 代表数据读取成功次数
                __sync_fetch_and_add(&process_count, 1);
                continue;
            }
            // 读取失败
            if (n < 0) {
                perror("read");
            }
            // 关闭套接字
            close(client);
            goto restart;
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // 设置默认端口
    int opt, port=DEFAULT_PORT;
    // 默认 worker 数量
    int num_thread=1;
    // 默认 busy_loop 数量
    int num_busy=0;

    // 分析参数
    while (-1 != (opt = getopt(argc, argv, "p:c:b:"))) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'c':
                num_thread = atoi(optarg);
                break;
            case 'b':
                num_busy = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return 1;
        }
    }

    listener = setup_server_socket(port);
    printf("Listening port (-p): %d\n", port);
    printf("Worker thread (-c): %d\n", num_thread);

    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * num_thread);

    for (int i = 0; i < num_thread; ++i) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    unsigned long prev=0, count=0;
    for (;;) {
        sleep(1);
        count = __sync_fetch_and_add(&process_count, 0);
        printf("processed %lu requests.\n", count-prev);
        prev = count;
    }

    return 0;
}