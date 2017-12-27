package com.muggle.example.netty.server;

import com.google.protobuf.Message;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;

public class ServerHandler extends ChannelInboundHandlerAdapter {
    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        System.out.println("connect from " + ctx.channel().remoteAddress());
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        System.out.println("disconnect " + ctx.channel().remoteAddress());
        ctx.close();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        Message message = (Message)msg;
        System.out.println(message.getDescriptorForType().getFullName());
        System.out.println(Networkpack.Ping.getDescriptor().getFullName());
        if (message.getDescriptorForType().getFullName().equals(Networkpack.Ping.getDescriptor().getFullName())) {
            Networkpack.Ping ping = (Networkpack.Ping)message;
            ctx.writeAndFlush(Networkpack.Pong.newBuilder().setReq(ping.getReq()).build());
        }
    }
}
