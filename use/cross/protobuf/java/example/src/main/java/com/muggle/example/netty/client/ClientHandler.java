package com.muggle.example.netty.client;

import com.google.protobuf.Message;
import gen.proto.Networkpack;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.concurrent.ScheduledFuture;

import java.util.concurrent.TimeUnit;

public class ClientHandler extends ChannelInboundHandlerAdapter {

    private ScheduledFuture heartbeatTaskFuture;
    private ScheduledFuture timeRecordTaskFuture;
    private ClientMessageDispatcher messageDispatcher = new ClientMessageDispatcher();

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        System.out.println("connected...");

        Channel channel = ctx.channel();

        // time record
        timeRecordTaskFuture = channel.eventLoop().scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                long ms = System.currentTimeMillis();

                Networkpack.TimeRecord.Builder timeRecordBuilder = Networkpack.TimeRecord.newBuilder();
                Networkpack.TimeSign.Builder timeSignBuilder = Networkpack.TimeSign.newBuilder();

                Networkpack.TimeSign timeSign = timeSignBuilder.setStartMs(ms).setEndMs(ms).build();
                channel.writeAndFlush(timeRecordBuilder.addSigns(timeSign).build());
            }
        }, 0, 100, TimeUnit.MILLISECONDS);

        // heartbeat
        heartbeatTaskFuture = channel.eventLoop().scheduleAtFixedRate(new Runnable() {
            int cnt = 0;
            Networkpack.Ping.Builder pingBuilder = Networkpack.Ping.newBuilder();

            @Override
            public void run() {
                channel.writeAndFlush(pingBuilder.setReq(cnt).build());
                ++cnt;
                if (cnt > 6) {
                    heartbeatTaskFuture.cancel(false);
                    timeRecordTaskFuture.cancel(false);
                    System.out.println("stop heartbeat and other task");
                }
            }
        }, 0, 5, TimeUnit.SECONDS);
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
