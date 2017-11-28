package com.muggle.helloproto;

import com.google.protobuf.Message;
import com.muggle.proto.Codec;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;
import javafx.geometry.Pos;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Hello {
    static public void main(String[] args) {
        if (args.length != 0) {
            if (args[0].equals("r")) {
                readSample();
            } else if (args[0].equals("w")) {
                writeSample();
            } else {
                System.out.println("enter: command [r|w]");
            }
        } else {
            baseSample();
        }
    }

    static private void baseSample() {
        MyReceiver receiver = new MyReceiver();
        Codec codec = new Codec();
        byte[] bytes = null;

        receiver.Register();

        // position
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        bytes = codec.Serialize(position);
        receiver.Parse(bytes);

        // rotation
        Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
        bytes = codec.Serialize(rotation);
        receiver.Parse(bytes);

        // Transform
        Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                .setPosition(position)
                .setRotation(rotation)
                .build();
        bytes = codec.Serialize(transform);
        receiver.Parse(bytes);

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
        bytes = codec.Serialize(lunchBuilder.build());
        receiver.Parse(bytes);

    }

    static private void readSample() {
        File f = new File("msg_bytes");
        long len = f.length();

        try (BufferedInputStream reader = new BufferedInputStream(new FileInputStream(f))){
            byte[] bytes = new byte[(int)len];
            reader.read(bytes);

            MyReceiver receiver = new MyReceiver();
            Codec codec = new Codec();
            receiver.Register();

            ByteBuffer byteBuffer = ByteBuffer.allocate(4);
            byteBuffer.order(ByteOrder.BIG_ENDIAN);

            int accum = 0;
            while (true) {
                byteBuffer.clear();
                byteBuffer.put(bytes, accum, 4);
                byteBuffer.clear();
                int messageLen = byteBuffer.getInt();

                byte[] messageBytes = new byte[messageLen];
                for (int i = 0; i < messageLen; ++i) {
                    messageBytes[i] = bytes[accum + i];
                }
                receiver.Parse(messageBytes);
                accum += messageLen;
                if (accum >= len) {
                    break;
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static private void writeSample() {
        Codec codec = new Codec();
        byte[] bytes = null;
        try (BufferedOutputStream writer = new BufferedOutputStream(new FileOutputStream("msg_bytes"))) {
            // position
            Gameobject.Position position = Gameobject.Position.newBuilder()
                    .setX(5.0f).setY(1.0f).setZ(6.0f).build();
            bytes = codec.Serialize(position);
            writer.write(bytes);

            // rotation
            Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                    .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
            bytes = codec.Serialize(rotation);
            writer.write(bytes);

            // scale
            Gameobject.Scale scale = Gameobject.Scale.newBuilder()
                    .setXScale(1.0f).setYScale(1.0f).setZScale(1.0f).build();
            bytes = codec.Serialize(scale);
            writer.write(bytes);

            // Transform
            Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                    .setPosition(position)
                    .setRotation(rotation)
                    .build();
            bytes = codec.Serialize(transform);
            writer.write(bytes);

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
            bytes = codec.Serialize(lunchBuilder.build());
            writer.write(bytes);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
