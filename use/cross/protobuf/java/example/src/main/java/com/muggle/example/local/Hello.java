package com.muggle.example.local;

import com.muggle.proto.codec.Code;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;

import java.io.*;

public class Hello {
    static private Code code;
    static private MessageHandler messageHandler;

    static public void main(String[] args) {
        Init();

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

    static private void Init() {
        code = new Code();
        messageHandler = new MessageHandler("proto/desc/proto.desc");
        messageHandler.Registers();
    }

    static private void baseSample() {
        byte[] bytes = null;

        // position
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        bytes = code.Serialize(position);
        messageHandler.OnMessage(bytes);

        // rotation
        Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
        bytes = code.Serialize(rotation);
        messageHandler.OnMessage(bytes);

        // Transform
        Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                .setPosition(position)
                .setRotation(rotation)
                .build();
        bytes = code.Serialize(transform);
        messageHandler.OnMessage(bytes);

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
        bytes = code.Serialize(lunchBuilder.build());
        messageHandler.OnMessage(bytes);

    }

    static private void readSample() {
        File f = new File("msg_bytes");
        long len = f.length();

        try (BufferedInputStream reader = new BufferedInputStream(new FileInputStream(f))){
            byte[] bytes = new byte[(int)len];
            reader.read(bytes);
            messageHandler.OnMessage(bytes);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static private void writeSample() {
        byte[] bytes = null;
        try (BufferedOutputStream writer = new BufferedOutputStream(new FileOutputStream("msg_bytes"))) {
            // position
            Gameobject.Position position = Gameobject.Position.newBuilder()
                    .setX(5.0f).setY(1.0f).setZ(6.0f).build();
            bytes = code.Serialize(position);
            writer.write(bytes);

            // rotation
            Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                    .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
            bytes = code.Serialize(rotation);
            writer.write(bytes);

            // scale
            Gameobject.Scale scale = Gameobject.Scale.newBuilder()
                    .setXScale(1.0f).setYScale(1.0f).setZScale(1.0f).build();
            bytes = code.Serialize(scale);
            writer.write(bytes);

            // Transform
            Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                    .setPosition(position)
                    .setRotation(rotation)
                    .build();
            bytes = code.Serialize(transform);
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
            bytes = code.Serialize(lunchBuilder.build());
            writer.write(bytes);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
