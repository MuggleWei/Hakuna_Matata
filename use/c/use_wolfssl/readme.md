# 使用wolfssl
本项目展示了wolfssl的tls用法

## 构建项目
先运行gen_cert.sh生成根证书、私钥和服务器的私钥、证书，再运行build脚本生成项目构建文件，接着去编译项目即可

## 例子简介
详见[example](./example/readme.md)
此项目中包含一个echo服务和一个发送时间的客户端，都使用wolfssl TLS对通信进行加密

## set_fd的问题
wolfSSL_set_fd函数并不区分平台，第二个参数是int型，也就是POSIX socket的类型，而在windows下，socket的类型为SOCKET，其定义为
```
// WinSock2.h

typedef UINT_PTR        SOCKET;
```
而对应的UINT_PTR的定义为
```
// basetsd.h

#if defined(_WIN64)
    typedef __int64 INT_PTR, *PINT_PTR;
    typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

    typedef __int64 LONG_PTR, *PLONG_PTR;
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int64

#else
    typedef _W64 int INT_PTR, *PINT_PTR;
    typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;

    typedef _W64 long LONG_PTR, *PLONG_PTR;
    typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int32

#endif
```
可以看到，当为Win64时，SOCKET的本质是一个无符号的64位整型，而将转为int型时，可能出现精度丢失的情况，那么在windows下是否会出现SOCKET的值超出int型的范围呢？  
通过查阅文档[Socket Data Type](https://docs.microsoft.com/en-us/windows/win32/winsock/socket-data-type-2)，其中有提到  
> Windows Sockets handles have no restrictions, other than that the value INVALID_SOCKET is not a valid socket. Socket handles may take any value in the range 0 to INVALID_SOCKET–1.  

查了一下INVALID_SOCKET的值为
```
// WinSock2.h

#define INVALID_SOCKET  (SOCKET)(~0)
```
可以看出SOCKET的可取值范围是大于int型的。单从此文档看来，从SOCKET直接转为int型是有问题的。  

又转头看了一下openssl，发现openssl在set_fd时也是int型，接着找到了这么一句注释
```
// openssl/include/internal/sockets.h

/*
 * Even though sizeof(SOCKET) is 8, it's safe to cast it to int, because
 * the value constitutes an index in per-process table of limited size
 * and not a real pointer. And we also depend on fact that all processors
 * Windows run on happen to be two's-complement, which allows to
 * interchange INVALID_SOCKET and -1.
 */
```

**综上所述: 虽然从类型定义以及微软的文档来看，直接将SOCKET当作int传入set_fd是有问题的，但是基于事实，在实践中这么做是被认为可行的**
