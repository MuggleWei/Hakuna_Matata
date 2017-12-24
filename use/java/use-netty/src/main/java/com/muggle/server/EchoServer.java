package com.muggle.server;

public class EchoServer {
    static public void main(String args[]) throws Exception {
        new Server("localhost", 10102).start(new EchoServerHandler(), null);
    }
}
