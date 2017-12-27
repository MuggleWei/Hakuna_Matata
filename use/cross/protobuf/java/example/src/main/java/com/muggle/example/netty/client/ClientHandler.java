package com.muggle.example.netty.client;

import com.google.protobuf.Message;
import gen.proto.Networkpack;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.concurrent.ScheduledFuture;

import java.util.concurrent.TimeUnit;

public class ClientHandler extends ChannelInboundHandlerAdapter {

    private ScheduledFuture timeTaskFuture;
    private ClientMessageDispatcher messageDispatcher = new ClientMessageDispatcher();

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        System.out.println("connected...");

        Channel channel = ctx.channel();
        timeTaskFuture = channel.eventLoop().scheduleAtFixedRate(new Runnable() {
            int cnt = 0;
            Networkpack.Ping.Builder pingBuilder = Networkpack.Ping.newBuilder();

            @Override
            public void run() {
                channel.writeAndFlush(pingBuilder.setReq(cnt).build());
                ++cnt;
                if (cnt > 3) {
                    timeTaskFuture.cancel(false);
                    System.out.println("stop heartbeat");
                }
            }
        }, 0, 1, TimeUnit.SECONDS);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        System.out.println("disconnected...");
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        System.out.println("caught disconnected...");
        cause.printStackTrace();
        ctx.close();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        Message message = (Message)msg;
        messageDispatcher.OnMessage(ctx, message);
    }
}
