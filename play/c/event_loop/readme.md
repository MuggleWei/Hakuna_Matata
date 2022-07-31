# EventLoop
本工程用于展示一些eventloop的例子

## example_linux/event
使用eventfd

## example_linux/pipe
用pipe模仿eventfd用于通知的效果

## example_linux/epoll_eventfd
使用epoll作为eventloop，用eventfd来在其他线程唤醒eventloop

## example_linux/select_eventfd
使用select作为eventloop，用eventfd来在其他线程唤醒eventloop

