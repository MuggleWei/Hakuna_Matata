package com.muggle.example.client;

import com.muggle.example.client.callback.PongCallback;
import com.muggle.example.client.callback.RspLoginCallback;
import com.muggle.example.client.callback.TimeRecordCallback;
import com.muggle.proto.handler.NettyMessageHandler;
import gen.proto.Networkpack;
import org.springframework.beans.factory.annotation.Autowired;

public class ClientMessageHandler extends NettyMessageHandler {

    @Autowired
    private PongCallback pongCallback;

    @Autowired
    private RspLoginCallback rspLoginCallback;

    @Autowired
    private TimeRecordCallback timeRecordCallback;

    public ClientMessageHandler(String descFileName) {
        super(descFileName);
    }

    public void Registers() {
        UseDefaultCallback();
        Register(Networkpack.Pong.newBuilder().build(), pongCallback);
        Register(Networkpack.RspLogin.newBuilder().build(), rspLoginCallback);
        Register(Networkpack.TimeRecord.newBuilder().build(), timeRecordCallback);
    }
}
