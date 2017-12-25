package com.muggle.server;

import io.netty.util.ResourceLeakDetector;

public class EchoServer {
    static public void main(String args[]) throws Exception {
        ResourceLeakDetector.setLevel(ResourceLeakDetector.Level.ADVANCED);
        new Server("localhost", 10102).start(new EchoServerHandler());
    }
}
