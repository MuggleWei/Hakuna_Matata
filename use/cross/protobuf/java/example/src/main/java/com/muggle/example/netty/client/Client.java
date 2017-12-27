package com.muggle.example.netty.client;

import com.muggle.protobuf.codec.netty.ProtobufDecoder;
import com.muggle.protobuf.codec.netty.ProtobufEncoder;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;
import gen.proto.Networkpack;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.net.InetSocketAddress;

public class Client {

    private String host;
    private int port;

    public Client(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public void start(EventLoopGroup group) throws Exception {
        while (true) {
            try {
                Bootstrap b = new Bootstrap();
                b.group(group)
                        .channel(NioSocketChannel.class)
                        .remoteAddress(new InetSocketAddress(host, port))
                        .handler(new ChannelInitializer<SocketChannel>() {
                            @Override
                            protected void initChannel(SocketChannel ch) throws Exception {
                                ProtobufDecoder decoder = new ProtobufDecoder();
                                decoder.AddDesc("proto/desc/proto.desc");
                                decoder.AddProtoType(Networkpack.Ping.getDefaultInstance());
                                decoder.AddProtoType(Networkpack.Pong.getDefaultInstance());

                                ch.pipeline().addLast(decoder);
                                ch.pipeline().addLast(new ProtobufEncoder());
                                ch.pipeline().addLast(new ClientHandler());
                            }
                        });
                ChannelFuture f = b.connect().sync();
                f.channel().closeFuture().sync();
            } catch (Exception e) {
                e.printStackTrace();
            }

            Thread.sleep(2000);
            System.out.println("try reconnect...");
        }

    }

    static public void main(String[] args) throws Exception {
        String host = "localhost";
        int port = 10102;

        EventLoopGroup group = new NioEventLoopGroup();
        new Client(host, port).start(group);
        group.shutdownGracefully().sync();
    }
}
