package com.muggle.example.server.gate.callback;

import com.google.protobuf.Message;
import com.muggle.proto.handler.NettyProtoCallback;
import com.muggle.proto.send.NettyWriter;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class ReqLoginCallback implements NettyProtoCallback {

    @Autowired
    private NettyWriter nettyWriter;

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.ReqLogin reqLogin = (Networkpack.ReqLogin)msg;
        System.out.println("receive message: reqLogin " + reqLogin.getUserName());
        Networkpack.ErrorInfo errorInfo = Networkpack.ErrorInfo.newBuilder()
                .setErrCode(0)
                .build();
        Networkpack.RspLogin rspLogin = Networkpack.RspLogin.newBuilder()
                .setUserName(reqLogin.getUserName())
                .setErrorInfo(errorInfo)
                .build();
        nettyWriter.writeAndFlush(ctx, rspLogin);
    }
}
