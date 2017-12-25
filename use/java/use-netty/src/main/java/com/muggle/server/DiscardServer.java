package com.muggle.server;

import io.netty.util.ResourceLeakDetector;

public class DiscardServer {

    static public void main(String args[]) throws Exception {
        ResourceLeakDetector.setLevel(ResourceLeakDetector.Level.ADVANCED);
        new Server("localhost", 10102).start(new DiscardServerHandler());
    }
}
