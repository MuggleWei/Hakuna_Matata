package com.muggle.example.client.callback;

import com.google.protobuf.Message;
import com.muggle.example.client.ClientLogic;
import com.muggle.proto.handler.NettyProtoCallback;
import gen.proto.Networkpack;
import io.netty.channel.ChannelHandlerContext;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class RspLoginCallback implements NettyProtoCallback {

    @Autowired
    private ClientLogic clientLogic;

    @Override
    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        Networkpack.RspLogin rspLogin = (Networkpack.RspLogin)msg;
        if (!rspLogin.hasErrorInfo() || rspLogin.getErrorInfo().getErrCode() == 0) {
            clientLogic.setConnStatus(ClientLogic.CONN_STATUS_LOGINED);
            System.out.println("success login");
        } else {
            clientLogic.setConnStatus(ClientLogic.CONN_STATUS_LOGIN_FAILED);
            System.out.println("failed login");
        }
    }
}
