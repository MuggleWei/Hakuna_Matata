package com.muggle.example.netty.client;

import com.google.protobuf.Message;
import com.muggle.protobuf.dispatcher.netty.ProtobufCallback;
import com.muggle.protobuf.dispatcher.netty.ProtobufDispatcher;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;

import java.util.ArrayList;

public class ClientMessageDispatcher extends ProtobufDispatcher {
    public ClientMessageDispatcher() {
        Registers();
    }

    public void Registers() {
        RegisterDefault(new DefaultCallback());
        Register(Networkpack.Pong.getDefaultInstance(), new PongCallback());
        Register(gen.proto.Timerecord.TimeRecord.getDefaultInstance(), new TimeRecordCallback());
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
        Networkpack.Pong pong = (Networkpack.Pong) msg;
        System.out.println("receive pong message: " + ((Networkpack.Pong) msg).getReq());
    }
}

class TimeRecordCallback implements ProtobufCallback {

    private gen.proto.Timerecord.TimeRecord.Builder builder = gen.proto.Timerecord.TimeRecord.newBuilder();
    private long peak = 0;
    private ArrayList<Long> elapseds = new ArrayList<>();

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        gen.proto.Timerecord.TimeRecord timeRecord = (gen.proto.Timerecord.TimeRecord) msg;
        long nanoTime = System.nanoTime();
        long elapsedMicroSeconds = (nanoTime - timeRecord.getTimeRecords(0)) / 1000;
        elapseds.add(elapsedMicroSeconds);

        int n = 100;
        if (elapseds.size() >= n) {
            long totalMicroSeconds = 0;
            long maxMicroSeconds = 0;
            for (Long elapsed : elapseds) {
                if (elapsed > maxMicroSeconds) {
                    maxMicroSeconds = elapsed;
                }
                totalMicroSeconds += elapsed;
            }
            long avg = totalMicroSeconds / elapseds.size();
            System.out.println("recently 100 messages avg elapsed: " + avg + " micro seconds");
            System.out.println("recently 100 messages max elapsed: " + maxMicroSeconds + " micro seconds");
            elapseds.clear();
        }
    }
}