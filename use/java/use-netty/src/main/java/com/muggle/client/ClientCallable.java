package com.muggle.client;

import io.netty.channel.EventLoopGroup;

import java.util.concurrent.Callable;

public class ClientCallable implements Callable<Integer> {

    private int id;
    private String host;
    private int port;
    private EventLoopGroup group;

    public ClientCallable(int id, String host, int port, EventLoopGroup group) {
        this.id = id;
        this.host = host;
        this.port = port;
        this.group = group;
    }

    @Override
    public Integer call() throws Exception {
        try {
            System.out.println("client: " + id + " start run -- from " + Thread.currentThread().getName());
            new Client(id, host, port).start(group);
        } catch (Exception e) {
            e.printStackTrace();
            return 1;
        }

        return 0;
    }
}
