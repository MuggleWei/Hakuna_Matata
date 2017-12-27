package com.muggle.protobuf.dispatcher.netty;

import com.google.protobuf.Message;
import io.netty.channel.ChannelHandlerContext;

public interface ProtobufCallback {
    void OnMessage(ChannelHandlerContext ctx, Message msg);
}
