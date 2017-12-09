package com.muggle.proto.handler;

import com.google.protobuf.Message;
import io.netty.channel.ChannelHandlerContext;

public interface NettyProtoCallback {
    void OnMessage(ChannelHandlerContext ctx, Message msg);
}

class DefaultCallback implements NettyProtoCallback {

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        System.out.println("receive message: " + msg.getDescriptorForType().getFullName());
    }
}