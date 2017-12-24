package com.muggle.client;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.CharsetUtil;

import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class ClientHandler extends ChannelInboundHandlerAdapter {

    private int id;

    public ClientHandler(int id) {
        this.id = id;
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        Channel channel = ctx.channel();
        ScheduledFuture<?> future = channel.eventLoop().scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                String msg = "message from client " + id;
                byte[] bytes = msg.getBytes(CharsetUtil.UTF_8);
                int len = bytes.length;

                ByteBuf byteBuf = Unpooled.buffer(4 + bytes.length);
                byteBuf.writeInt(len);
                byteBuf.writeBytes(bytes);

                channel.writeAndFlush(byteBuf);

                System.out.println("send message: " + msg);
            }
        }, 1, 1, TimeUnit.SECONDS);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        System.out.println("caught disconnected...");
        ctx.close();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        System.out.println("client " + id + " received: " + (String)msg);
    }
}