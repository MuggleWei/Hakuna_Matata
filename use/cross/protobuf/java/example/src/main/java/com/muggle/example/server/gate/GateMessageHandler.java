package com.muggle.example.server.gate;

import com.muggle.example.server.gate.callback.PingCallback;
import com.muggle.example.server.gate.callback.ReqLoginCallback;
import com.muggle.proto.handler.NettyMessageHandler;
import gen.proto.Networkpack;
import org.springframework.beans.factory.annotation.Autowired;

public class GateMessageHandler extends NettyMessageHandler {

    @Autowired
    private PingCallback pingCallback;

    @Autowired
    private ReqLoginCallback reqLoginCallback;

    public GateMessageHandler(String descFileName) {
        super(descFileName);
    }

    public void Registers() {
        UseDefaultCallback();
        Register(Networkpack.Ping.newBuilder().build(), pingCallback);
        Register(Networkpack.ReqLogin.newBuilder().build(), reqLoginCallback);
    }
}
