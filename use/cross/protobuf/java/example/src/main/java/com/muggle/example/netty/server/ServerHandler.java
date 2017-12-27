package com.muggle.example.netty.server;

import com.google.protobuf.Message;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;

@ChannelHandler.Sharable
public class ServerHandler extends ChannelInboundHandlerAdapter {

    ServerMessageDispatcher dispatcher = new ServerMessageDispatcher();

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        System.out.println("connect from " + ctx.channel().remoteAddress());
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        System.out.println("exceptionCaught " + ctx.channel().remoteAddress());
        cause.printStackTrace();
        ctx.close();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        Message message = (Message)msg;
        dispatcher.OnMessage(ctx, message);
    }
}
