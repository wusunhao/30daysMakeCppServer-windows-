# 01day知识点总结

## 1.server里的知识点

### 1.对于Winsock的初始化

```cpp
WSADATA data;
```

`WSADATA` 是一个结构体，用来存储有关 Winsock 库的版本信息和初始化状态。在成功调用 `WSAStartup` 后，这个结构体会被填充一些关于当前 Winsock 实现的信息，比如支持的 Winsock 版本。

```cpp
if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    printf("WSAStartup失败\n");
    return 1;
}
```

`WSAStartup` 是一个 Winsock 库初始化函数。它的作用是启动 Windows 套接字库，并告知程序希望使用的 Winsock 版本（通过 `MAKEWORD(2, 2)` 指定）。如果初始化成功，`WSAStartup` 返回 0，并且 `data` 结构体会被填充。

如果初始化失败，`WSAStartup` 会返回非零值，此时可以通过检查返回值来处理错误。

`MAKEWORD(2, 2)` 是一个宏，它将传入的两个参数（主版本号和次版本号）合并成一个 16 位的字。我们这里用的Winsock版本是2.2版本

#### 知识点:为什么需要使用WSAStartup以及什么是Winsock?

Windows 操作系统提供了一个套接字API，称为 **Winsock**，它用于处理网络编程。`WSAStartup` 是这个 API 的初始化函数，类似于其他平台上网络编程所需的库初始化函数。你需要在使用任何与网络相关的功能之前调用它。

Winsock是 Windows 操作系统上的一种网络编程接口，提供了一套用于开发网络应用程序的 API（应用程序编程接口）。它使得 Windows 程序能够进行网络通信，包括与其他计算机或设备之间的 TCP/IP 协议栈进行数据交换。

### 2.对于套接字的创建以及检测

```cpp
SOCKET socketword = socket(AF_INET, SOCK_STREAM, 0);
if (socketword == INVALID_SOCKET) {
    printf("套接字创建失败: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
}
```

在 Winsock 中，套接字（socket）是由 SOCKET类型表示的。它是一个标识符，操作系统用它来管理和跟踪打开的网络连接。

`socket()函数`的用法:

```cpp
SOCKET socket(int af,int type,int protocol);
```

**`af`**(地址族)：指定套接字使用的地址类型。`AF_INET` 表示使用 **IPv4** 地址。对于 IPv6 地址族，可以使用 `AF_INET6`。

**`type`**(套接字类型)：指定套接字的通信类型。`SOCK_STREAM` 表示创建一个 **TCP** 套接字（面向连接的流式套接字）。如果你希望使用 **UDP**，则可以使用 `SOCK_DGRAM`。

**`protocol`**(指定协议类型)：通常情况下，传递 `0` 表示使用与套接字类型关联的默认协议。例如，`SOCK_STREAM` 默认使用 **TCP** 协议，`SOCK_DGRAM` 默认使用 **UDP** 协议。

如果成功，`socket()` 返回一个有效的套接字描述符（SOCKET类型）。这个套接字可以用于后续的网络操作。

如果失败，返回 INVALID_SOCKET，表示套接字创建失败。

**`WSAGetLastError()`**函数，用于获取 **最近的 Winsock 错误代码**。当某个 Winsock 操作失败时，错误代码会存储在 Winsock 库内部，`WSAGetLastError()` 可以用来查询该错误代码。

`WSACleanup()` 函数，用于清理 Winsock 库。通常情况下，在 Winsock 库使用完成后，需要调用 `WSACleanup()` 来释放资源。

### 3.创建结构体来存放服务器的地址

```cpp
struct sockaddr_in serv_addr;
ZeroMemory(&serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
serv_addr.sin_port = htons(8888);
```

`sockaddr_in` 是一个结构体，专门用于存储 **IPv4 地址** 和相关信息。

```cpp
struct sockaddr_in {
    short sin_family;         // 地址族
    unsigned short sin_port;  // 端口号
    struct in_addr sin_addr;  // IP 地址
    char sin_zero[8];         // 填充字段，通常为 0
};
```

**`sin_family`**：指定地址族，通常是 `AF_INET`，表示使用 **IPv4**。

**`sin_port`**：指定端口号。

**`sin_addr`**：指定 IP 地址。

**`sin_zero`**：填充字段，通常是 0，保持结构体大小与其他 `sockaddr` 类型结构一致。

**`ZeroMemory`** 函数将 `serv_addr` 结构体的内存清零。相当于`memset(&serv_addr,0,sizeof(serv_addr));`

### 4.绑定一个套接字到指定的IP地址和端口

```cpp
if (bind(socketword, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    printf("绑定失败: %d\n", WSAGetLastError());
    closesocket(socketword);
    WSACleanup();
    return 1;
}
```

`bind()`函数的用法

```cpp
int bind(SOCKET s, const struct sockaddr *addr, int addrlen);
```

**`s`**：要绑定的套接字描述符（即你通过 `socket()` 创建的套接字）。

**`addr`**：一个指向 `sockaddr` 类型结构体的指针。

**`addrlen`**：结构体的大小,我们在这里用`sizeof()`函数;

`bind()`函数用于将套接字与本地地址（IP 地址和端口号）绑定的函数。

**`SOCKET_ERROR`** 是一个常量，表示函数调用失败。

**`closesocket()`**：用于关闭套接字。在 `bind()` 失败后，程序会关闭套接字以释放相关资源。

### 5.监听服务器套接字，等待连接请求

```cpp
if (listen(socketword, SOMAXCONN) == SOCKET_ERROR) {
    printf("监听失败: %d\n", WSAGetLastError());
    closesocket(socketword);
    WSACleanup();
    return 1;
}
```

`listen()`函数的用法：

```cpp
int listen(SOCKET s, int backlog);
```

**`s`**：传入需要监听的套接字

**`backlog`**：表示在连接队列中等待的最大客户端连接数，即最大连接数。

`SOMAXCONN` 是一个宏，表示最大连接数，通常是一个大值，表示系统默认的最大连接数。

### 6.创建结构体来存放客户端的地址信息

```cpp
struct sockaddr_in clnt_addr;
int clnt_addr_len = sizeof(clnt_addr);
ZeroMemory(&clnt_addr,sizeof(clnt_addr));
```

跟上面5的操作差不多，同时你初始化为0的时候你想用`memset()`还是`ZeroMemory()`都行

`int clnt_addr_len`用来给`accept(`)函数指示地址结构的大小

### 7.接受客户端的连接请求

```cpp
SOCKET clnt_socket_word = accept(socketword, (sockaddr*)&clnt_addr, &clnt_addr_len);
if (clnt_socket_word == INVALID_SOCKET) {
    printf("接收失败: %d\n", WSAGetLastError());
    closesocket(socketword);
    WSACleanup();
    return 1;
}
```

`accept()`函数的用法：

```cpp
SOCKET accept(SOCKET s, struct sockaddr *addr, int *addrlen);
```

**`s`**：传入的监听套接字

**`addr`**：这是一个指向 `sockaddr` 类型的指针,用于存储客户端的地址信息。

**`addrlen`**：这是一个指向整数的指针，表示客户端地址的长度。

这个返回值跟上面的`socket`函数一样。

### 8.打印客户端连接的IP地址和端口号

```cpp
printf("新客户端已连接！IP: %s Port: %d\n",inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));
```

`inet_ntoa()`函数的用法

```cpp
char *inet_ntoa(struct in_addr in);
```

**`in`**：这是一个 `struct in_addr` 类型的参数，表示网络字节序的IPv4 地址。

将一个 **网络字节序** 的 32 位 **IPv4 地址** 转换为一个字符串表示的 **点分十进制格式**

`ntohs()`函数的用法

```cpp
uint16_t ntohs(uint16_t netshort);
```

这是一个将 **网络字节序**（大端字节序）转换为 **主机字节序**（可能是小端字节序）的函数。`ntohs()` 函数用于转换端口号

## 2.cilent里的知识点

前面的跟上面的一样的，就是把accept换成了connect

### 1.客户端连接到服务器

```cpp
if (connect(socketword, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    printf("连接失败: %d\n", WSAGetLastError());
    closesocket(socketword);
    WSACleanup();
    return 1;
}
```

`connect()`函数的用法

```cpp
int connect(SOCKET sockfd,const struct sockaddr *addr,int addrlen);
```

**`s`**：传入的监听套接字

**`addr`**：这是一个指向 `sockaddr` 类型的指针,用于存储客户端的地址信息。

**`addrlen`**：这是一个指向整数的指针，表示客户端地址的长度。

