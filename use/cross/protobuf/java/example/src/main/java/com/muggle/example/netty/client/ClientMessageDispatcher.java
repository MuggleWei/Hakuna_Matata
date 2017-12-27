package com.muggle.example.netty.client;

import com.google.protobuf.Message;
import com.muggle.protobuf.dispatcher.netty.ProtobufCallback;
import com.muggle.protobuf.dispatcher.netty.ProtobufDispatcher;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;

public class ClientMessageDispatcher extends ProtobufDispatcher {
    public ClientMessageDispatcher() {
        Registers();
    }

    public void Registers() {
        RegisterDefault(new DefaultCallback());
        Register(Networkpack.Pong.getDefaultInstance(), new PongCallback());
    }
}

class DefaultCallback implements ProtobufCallback {
    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        System.out.println("on message: " + msg.getDescriptorForType().getFullName());
    }
}

class PongCallback implements ProtobufCallback {

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.Pong pong = (Networkpack.Pong)msg;
        System.out.println("receive pong message: " + ((Networkpack.Pong) msg).getReq());
    }
}
