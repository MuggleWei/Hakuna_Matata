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
        Register(Networkpack.TimeRecord.getDefaultInstance(), new TimeRecordCallback());
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

class TimeRecordCallback implements ProtobufCallback {

    private Networkpack.TimeSign.Builder builder = Networkpack.TimeSign.newBuilder();
    private long peak = 0;
    private int accumulateCnt = 0;
    private long accumulateMs = 0;

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.TimeRecord timeRecord = (Networkpack.TimeRecord)msg;
        long ms = System.currentTimeMillis();
        long elapsedMs = ms - timeRecord.getSigns(0).getStartMs();
        if (elapsedMs > peak) {
            peak = elapsedMs;
            System.out.println(" -- new peak elapsed millisecond: " + peak);
        }

        ++accumulateCnt;
        accumulateMs += elapsedMs;
        if (accumulateCnt > 20) {
            double avg = (double)accumulateMs / (double)accumulateCnt;
            System.out.println("last 20 record avg elapsed ms: " + avg);
            accumulateMs = 0;
            accumulateCnt = 0;
        }
    }
}