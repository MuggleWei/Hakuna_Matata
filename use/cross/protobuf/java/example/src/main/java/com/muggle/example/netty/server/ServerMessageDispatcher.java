package com.muggle.example.netty.server;

import com.google.protobuf.Message;
import com.muggle.protobuf.dispatcher.netty.ProtobufCallback;
import com.muggle.protobuf.dispatcher.netty.ProtobufDispatcher;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;

public class ServerMessageDispatcher extends ProtobufDispatcher {

    public ServerMessageDispatcher() {
        Registers();
    }

    public void Registers() {
        RegisterDefault(new DefaultCallback());
        Register(Networkpack.Ping.getDefaultInstance(), new PingCallback());
        Register(gen.proto.Timerecord.TimeRecord.getDefaultInstance(), new TimeRecordCallback());
    }
}

class DefaultCallback implements ProtobufCallback {
    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        System.out.println("on message: " + msg.getDescriptorForType().getFullName());
    }
}

class PingCallback implements ProtobufCallback {

    Networkpack.Pong.Builder builder = Networkpack.Pong.newBuilder();

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.Ping ping = (Networkpack.Ping)msg;
        System.out.println("receive ping message: " + ping.getReq());
        Networkpack.Pong pong = builder.setReq(ping.getReq()).build();
        ctx.writeAndFlush(pong);
    }
}

class TimeRecordCallback implements ProtobufCallback {

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        ctx.writeAndFlush(msg);
    }
}