package com.muggle.example.client.callback;

import com.google.protobuf.Message;
import com.muggle.proto.handler.NettyProtoCallback;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import org.springframework.stereotype.Component;

@Component
public class PongCallback implements NettyProtoCallback {

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.Pong pong = (Networkpack.Pong)msg;
        System.out.println("get message: pong " + pong.getReq());
    }
}
