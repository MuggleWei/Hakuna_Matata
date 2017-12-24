package com.muggle.server;

public class DiscardServer {

    static public void main(String args[]) throws Exception {
        new Server("localhost", 10102).start(new DiscardServerHandler(), null);
    }
}
