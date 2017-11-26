package com.muggle.helloproto;

import com.google.protobuf.AbstractMessage;
import com.google.protobuf.DynamicMessage;
import com.sun.org.apache.bcel.internal.classfile.Code;
import gen.proto.Gameobject;

import java.io.*;

public class Hello {
    static public void main(String[] args) {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(1.0f).setY(1.0f).setZ(1.0f).build();

        Codec codec = new Codec();
        byte[] bytes = codec.Serialize(position);

        try (BufferedOutputStream writer = new BufferedOutputStream(new FileOutputStream("msg_bytes"))) {
            writer.write(bytes);
        } catch (IOException e) {
            e.printStackTrace();
        }

        DynamicMessage msg = (DynamicMessage) codec.Deserialize(bytes);
        System.out.println(msg.getDescriptorForType().getFullName());

    }
}
