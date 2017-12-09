package com.muggle.example.client.config;

import com.muggle.example.client.Client;
import com.muggle.example.client.ClientHandler;
import com.muggle.example.client.ClientLogic;
import com.muggle.example.client.ClientMessageHandler;
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
        value = {"file:example/config/client.properties"},
        ignoreResourceNotFound = true)
@ComponentScan(basePackages = "com.muggle.example.client")
public class ClientConfig implements InitializingBean {

    @Autowired
    Environment env;

    @Bean(name = "client")
    public Client client() {
        String host = env.getProperty("gate.host", "localhost");
        Integer port = env.getProperty("gate.port", Integer.class, 10102);
        Client client = new Client();
        client.setHost(host);
        client.setPort(port);
        return client;
    }

    @Bean(name = "clientHandler")
    public ClientHandler clientHandler() {
        return new ClientHandler();
    }

    @Bean(name = "messageHandler")
    public ClientMessageHandler messageHandler() {
        return new ClientMessageHandler("proto/desc/proto.desc");
    }

    @Bean(name = "nettyWriter")
    public NettyWriter nettyWriter() {
        return new NettyWriter();
    }

    @Bean(name = "clientLogic")
    ClientLogic clientLogic() {
        return new ClientLogic();
    }

    @Override
    public void afterPropertiesSet() throws Exception {
        ClientMessageHandler clientMessageHandler = messageHandler();
        clientMessageHandler.Registers();
    }
}
