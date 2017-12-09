package com.muggle.example.client;

import com.muggle.proto.codec.Code;
import com.muggle.proto.send.NettyWriter;
import gen.proto.Networkpack;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.util.HashedWheelTimer;
import io.netty.util.Timeout;
import io.netty.util.Timer;
import io.netty.util.TimerTask;
import io.netty.util.concurrent.ScheduledFuture;
import org.springframework.beans.factory.annotation.Autowired;

import java.util.concurrent.TimeUnit;

public class ClientHandler extends ChannelInboundHandlerAdapter {

    @Autowired
    private ClientMessageHandler messageHandler;

    @Autowired
    private ClientLogic clientLogic;

    @Autowired
    private NettyWriter nettyWriter;

    private ScheduledFuture sf;
    private int requestId = 0;

    @Override
    public void channelActive(ChannelHandlerContext ctx)
    {
        System.out.println("connected! "+ctx.channel());
        clientLogic.setConnStatus(ClientLogic.CONN_STATUS_CONNECTTED);

        // ping
        sf = ctx.executor().scheduleAtFixedRate(() -> {
            ++requestId;
            Networkpack.Ping ping = Networkpack.Ping.newBuilder().setReq(requestId).build();
            nettyWriter.writeAndFlush(ctx, ping);
            System.out.println("send ping");
        }, 1, 1, TimeUnit.SECONDS);

        // login
        Timer timer = new HashedWheelTimer();
        timer.newTimeout(new TimerTask() {
            public void run(Timeout timeout) throws Exception {
                Networkpack.ReqLogin reqLogin = Networkpack.ReqLogin.newBuilder()
                        .setUserName("muggle")
                        .setPasswd("123456")
                        .build();
                nettyWriter.writeAndFlush(ctx, reqLogin);
                System.out.println("send login req");
            }
        }, 3, TimeUnit.SECONDS);
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) {
        System.out.println("disconnected! "+ctx.channel());
        clientLogic.setConnStatus(ClientLogic.CONN_STATUS_UNCONNECT);

        sf.cancel(false);
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        ByteBuf in = (ByteBuf) msg;
        messageHandler.OnMessage(ctx, in);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        System.out.println("exceptionCaught: " + ctx.name());
        ctx.close();
    }
}
