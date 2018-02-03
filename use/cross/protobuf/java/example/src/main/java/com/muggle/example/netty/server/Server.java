package com.muggle.example.netty.server;

import com.muggle.example.netty.client.ClientHandler;
import com.muggle.protobuf.codec.netty.ProtobufDecoder;
import com.muggle.protobuf.codec.netty.ProtobufEncoder;
import gen.proto.Networkpack;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.handler.timeout.IdleStateHandler;

import java.net.InetSocketAddress;
import java.util.concurrent.TimeUnit;

public class Server {
    private String host;
    private int port;

    public Server(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public void start(ChannelHandler handler) throws Exception {
        EventLoopGroup group = new NioEventLoopGroup();

        try {
            ServerBootstrap b = new ServerBootstrap();
            b.group(group)
                    .channel(NioServerSocketChannel.class)
                    .localAddress(new InetSocketAddress(host, port))
                    .childHandler(new ChannelInitializer<SocketChannel>() {
                        @Override
                        protected void initChannel(SocketChannel ch) throws Exception {
                            ProtobufDecoder decoder = new ProtobufDecoder();
                            decoder.AddDesc("proto/desc/proto.desc");
                            decoder.AddProtoType(Networkpack.Ping.getDefaultInstance());
                            decoder.AddProtoType(Networkpack.Pong.getDefaultInstance());
                            decoder.AddProtoType(gen.proto.Timerecord.TimeRecord.getDefaultInstance());

                            ch.pipeline().addLast(new IdleStateHandler(0, 0, 5, TimeUnit.SECONDS));
                            ch.pipeline().addLast(new IdleTimeoutHandler());
                            ch.pipeline().addLast(decoder);
                            ch.pipeline().addLast(new ProtobufEncoder());
                            ch.pipeline().addLast(handler);
                        }
                    });
            ChannelFuture f = b.bind().sync();
            f.channel().closeFuture().sync();
        } finally {
            group.shutdownGracefully().sync();
        }
    }

    static public void main(String args[]) throws Exception {
        new Server("localhost", 10102).start(new ServerHandler());
    }
}
