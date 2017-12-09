package com.muggle.example.server.gate.callback;

import com.google.protobuf.Message;
import com.muggle.proto.handler.NettyProtoCallback;
import com.muggle.proto.send.NettyWriter;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class PingCallback implements NettyProtoCallback {

    @Autowired
    private NettyWriter nettyWriter;

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.Ping ping = (Networkpack.Ping)msg;
        System.out.println("receive message: ping " + ping.getReq());
        Networkpack.Pong pong = Networkpack.Pong.newBuilder()
                .setReq(ping.getReq())
                .build();
        nettyWriter.writeAndFlush(ctx, pong);
    }
}
