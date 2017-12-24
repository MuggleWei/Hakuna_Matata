package com.muggle.client;

import com.muggle.MessageDecode;
import com.muggle.MessageEncode;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class Client {

    private int id;
    private String host;
    private int port;

    public Client(int id, String host, int port) {
        this.id = id;
        this.host = host;
        this.port = port;
    }

    public void start(EventLoopGroup group) throws Exception {
        try {
            Bootstrap b = new Bootstrap();
            b.group(group)
                    .channel(NioSocketChannel.class)
                    .remoteAddress(new InetSocketAddress(host, port))
                    .handler(new ChannelInitializer<SocketChannel>() {
                        @Override
                        protected void initChannel(SocketChannel ch) throws Exception {
                            System.out.println("Init channel " + id);
                            ch.pipeline().addLast(
                                    new MessageDecode(),
                                    new MessageEncode(),
                                    new ClientHandler(id));
                        }
                    });
            ChannelFuture f = b.connect().sync();
            f.channel().closeFuture().sync();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    static public void main(String[] args) throws Exception {
        String host = "localhost";
        int port = 10102;
        EventLoopGroup group = new NioEventLoopGroup();

        List<Future<Integer>> resultList = new ArrayList<>();
        ExecutorService executorService = Executors.newCachedThreadPool();
        for (int i = 0; i < 10; i++){
            Thread.sleep(500);
            Future<Integer> future = executorService.submit(new ClientCallable(i, host, port, group));
            resultList.add(future);
        }
        executorService.shutdown();

        for (Future<Integer> future : resultList) {
            future.get();
        }
        group.shutdownGracefully().sync();
    }
}
