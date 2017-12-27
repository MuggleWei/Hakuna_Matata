package com.muggle.example.local;

import com.google.protobuf.Message;
import com.muggle.protobuf.codec.local.ProtobufDecoder;
import com.muggle.protobuf.codec.local.ProtobufEncoder;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class Hello {
    private ProtobufEncoder encoder = null;
    private ProtobufDecoder decoder = null;
    private MessageDispatcher dispatcher = null;

    static public void main(String[] args) {

        Hello hello = new Hello();
        hello.Init();

        if (args.length != 0) {
            if (args[0].equals("r")) {
                hello.readSample();
            } else if (args[0].equals("w")) {
                hello.writeSample();
            } else {
                System.out.println("enter: command [r|w]");
            }
        } else {
            hello.baseSample();
        }
    }

    public void Init() {
        encoder = new ProtobufEncoder();
        decoder = new ProtobufDecoder();
        dispatcher = new MessageDispatcher();

        decoder.AddDesc("proto/desc/proto.desc");
        decoder.AddProtoType(Gameobject.Position.newBuilder().build());
        decoder.AddProtoType(Gameobject.Rotation.newBuilder().build());
        decoder.AddProtoType(Gameobject.Transform.newBuilder().build());
        decoder.AddProtoType(FoodOuterClass.Lunch.newBuilder().build());
    }

    public void baseSample() {
        byte[] bytes = null;
        List<Message> messageList = null;

        // position
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        bytes = encoder.encode(position);
        messageList = decoder.decode(bytes);
        for (Message message : messageList) {
            dispatcher.OnMessage(message);
        }


        // rotation
        Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
        bytes = encoder.encode(rotation);
        messageList = decoder.decode(bytes);
        for (Message message : messageList) {
            dispatcher.OnMessage(message);
        }

        // Transform
        Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                .setPosition(position)
                .setRotation(rotation)
                .build();
        bytes = encoder.encode(transform);
        messageList = decoder.decode(bytes);
        for (Message message : messageList) {
            dispatcher.OnMessage(message);
        }

        // lunch
        FoodOuterClass.Lunch.Builder lunchBuilder = FoodOuterClass.Lunch.newBuilder();

        FoodOuterClass.Rice rice = FoodOuterClass.Rice.newBuilder().setOriginPlace("fujian").build();
        FoodOuterClass.Food foodRice = FoodOuterClass.Food.newBuilder().setRice(rice).build();
        lunchBuilder.addFoods(foodRice);

        FoodOuterClass.Noodle noodle = FoodOuterClass.Noodle.newBuilder()
                .setMaterial("flour")
                .setTaste(FoodOuterClass.Taste.NO_SPICY)
                .build();
        FoodOuterClass.Food foodNoodle = FoodOuterClass.Food.newBuilder().setNoodle(noodle).build();
        lunchBuilder.addFoods(foodNoodle);

        FoodOuterClass.Dumpling dumpling = FoodOuterClass.Dumpling.newBuilder()
                .setStuffing("Leek pork")
                .setTaste(FoodOuterClass.Taste.SPICY)
                .build();
        FoodOuterClass.Food foodDumpling = FoodOuterClass.Food.newBuilder().setDumpling(dumpling).build();
        lunchBuilder.addFoods(foodDumpling);

        lunchBuilder.addFoods(FoodOuterClass.Food.newBuilder().build());
        bytes = encoder.encode(lunchBuilder.build());
        messageList = decoder.decode(bytes);
        for (Message message : messageList) {
            dispatcher.OnMessage(message);
        }

    }

    public void readSample() {
        File f = new File("msg_bytes");
        long len = f.length();

        try (BufferedInputStream reader = new BufferedInputStream(new FileInputStream(f))){
            byte[] bytes = new byte[(int)len];
            reader.read(bytes);
            List<Message> messageList = decoder.decode(bytes);
            for (Message message : messageList) {
                dispatcher.OnMessage(message);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void writeSample() {
        byte[] bytes = null;
        List<Message> messageList = null;

        try (BufferedOutputStream writer = new BufferedOutputStream(new FileOutputStream("msg_bytes"))) {
            // position
            Gameobject.Position position = Gameobject.Position.newBuilder()
                    .setX(5.0f).setY(1.0f).setZ(6.0f).build();
            writer.write(encoder.encode(position));

            // rotation
            Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                    .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
            writer.write(encoder.encode(rotation));

            // scale
            Gameobject.Scale scale = Gameobject.Scale.newBuilder()
                    .setXScale(1.0f).setYScale(1.0f).setZScale(1.0f).build();
            writer.write(encoder.encode(scale));

            // Transform
            Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                    .setPosition(position)
                    .setRotation(rotation)
                    .build();
            writer.write(encoder.encode(transform));

            // lunch
            FoodOuterClass.Lunch.Builder lunchBuilder = FoodOuterClass.Lunch.newBuilder();

            FoodOuterClass.Rice rice = FoodOuterClass.Rice.newBuilder().setOriginPlace("fujian").build();
            FoodOuterClass.Food foodRice = FoodOuterClass.Food.newBuilder().setRice(rice).build();
            lunchBuilder.addFoods(foodRice);

            FoodOuterClass.Noodle noodle = FoodOuterClass.Noodle.newBuilder()

                    .setMaterial("flour")
                    .setTaste(FoodOuterClass.Taste.NO_SPICY)
                    .build();
            FoodOuterClass.Food foodNoodle = FoodOuterClass.Food.newBuilder().setNoodle(noodle).build();
            lunchBuilder.addFoods(foodNoodle);

            FoodOuterClass.Dumpling dumpling = FoodOuterClass.Dumpling.newBuilder()
                    .setStuffing("Leek pork")
                    .setTaste(FoodOuterClass.Taste.SPICY)
                    .build();
            FoodOuterClass.Food foodDumpling = FoodOuterClass.Food.newBuilder().setDumpling(dumpling).build();
            lunchBuilder.addFoods(foodDumpling);

            lunchBuilder.addFoods(FoodOuterClass.Food.newBuilder().build());
            writer.write(encoder.encode(lunchBuilder.build()));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
