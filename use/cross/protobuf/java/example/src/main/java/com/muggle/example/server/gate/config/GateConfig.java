package com.muggle.example.server.gate.config;

import com.muggle.example.server.gate.Gate;
import com.muggle.example.server.gate.GateHandler;
import com.muggle.example.server.gate.GateMessageHandler;
import com.muggle.proto.codec.Code;
import com.muggle.proto.send.NettyWriter;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.PropertySource;
import org.springframework.core.env.Environment;

@Configuration
@PropertySource(
        value = {"file:example/config/gate.properties"},
        ignoreResourceNotFound = true)
@ComponentScan(basePackages = "com.muggle.example.server.gate")
public class GateConfig implements InitializingBean {
    @Autowired
    Environment env;

    @Bean(name = "gate")
    Gate gate() {
        Integer port = env.getProperty("gate.port", Integer.class, 10102);
        Gate gate = new Gate();
        gate.setPort(port);
        return gate;
    }

    @Bean(name = "gateHandler")
    GateHandler gateHandler() {
        return new GateHandler();
    }

    @Bean(name = "gateMessageHandler")
    GateMessageHandler messageHandler() {
        return new GateMessageHandler("proto/desc/proto.desc");
    }

    @Bean(name = "nettyWriter")
    NettyWriter nettyWriter() {
        return new NettyWriter();
    }

    @Override
    public void afterPropertiesSet() throws Exception {
        GateMessageHandler gateMessageHandler = messageHandler();
        gateMessageHandler.Registers();
    }
}
