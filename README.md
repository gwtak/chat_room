升级版：https://github.com/gwtak/ChatRoom_Epoll

Linux环境下运行通过

已知问题：

1、在输入消息时，会被别人打断

2、client断开时，未关闭socket，不断发送空消息

![Image text](/20200429104738.png)

![Image text](/20200429104842.png)
