package com.muggle.example.client.callback;

import com.google.protobuf.Message;
import com.muggle.proto.handler.NettyProtoCallback;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import org.springframework.stereotype.Component;

@Component
public class TimeRecordCallback implements NettyProtoCallback {
    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.TimeRecord timeRecord = (Networkpack.TimeRecord)msg;
        for (Networkpack.TimeSign timeSign : timeRecord.getSignsList()) {
            System.out.print(timeSign.getStartMs() + " - " + timeSign.getEndMs() + " | ");
        }
    }
}
