package com.muggle;

import com.google.protobuf.Message;
import com.muggle.protobuf.codec.common.DecodeError;
import com.muggle.protobuf.codec.local.ProtobufDecoder;
import com.muggle.protobuf.codec.local.ProtobufEncoder;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.nio.ByteBuffer;
import java.util.List;

public class TestLocal {
    private ProtobufEncoder encoder = null;
    private ProtobufDecoder decoder = null;

    @Before
    public void init() {
        encoder = new ProtobufEncoder();
        decoder = new ProtobufDecoder();
        decoder.AddDesc("proto/desc/proto.desc");
        decoder.AddProtoType(Gameobject.Position.newBuilder().build());
        decoder.AddProtoType(Gameobject.Rotation.newBuilder().build());
        decoder.AddProtoType(Gameobject.Transform.newBuilder().build());
        decoder.AddProtoType(FoodOuterClass.Lunch.newBuilder().build());
    }

    @Test
    public void testPositionWithArg() {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        byte[] bytes = encoder.encode(position);

        List<Message> messageList = decoder.decode(bytes);
        Assert.assertNotNull(messageList);
        Assert.assertEquals(messageList.size(), 1);

        Message message = messageList.get(0);
        Assert.assertTrue(message instanceof Gameobject.Position);

        Gameobject.Position position1 = (Gameobject.Position) message;
        Assert.assertTrue(position.getX() == position1.getX());
        Assert.assertTrue(position.getY() == position1.getY());
        Assert.assertTrue(position.getZ() == position1.getZ());
    }

    @Test
    public void testPositionWithoutArg() {
        Gameobject.Position position = Gameobject.Position.newBuilder().build();
        byte[] bytes = encoder.encode(position);

        List<Message> messageList = decoder.decode(bytes);
        Assert.assertNotNull(messageList);
        Assert.assertEquals(messageList.size(), 1);

        Message message = messageList.get(0);
        Assert.assertTrue(message instanceof Gameobject.Position);

        Gameobject.Position position1 = (Gameobject.Position) message;
    }

    @Test
    public void testTrasform() {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        Gameobject.Rotation rotation = Gameobject.Rotation.newBuilder()
                .setXAxis(3.14f).setYAxis(0.0f).setZAxis(1.57f).build();
        Gameobject.Transform transform = Gameobject.Transform.newBuilder()
                .setPosition(position)
                .setRotation(rotation)
                .build();

        byte[] bytes = encoder.encode(transform);

        List<Message> messageList = decoder.decode(bytes);
        Assert.assertNotNull(messageList);
        Assert.assertEquals(messageList.size(), 1);

        Message message = messageList.get(0);
        Assert.assertTrue(message instanceof Gameobject.Transform);
        Gameobject.Transform transform1 = (Gameobject.Transform) message;
        Assert.assertTrue(transform1.hasPosition());
        Assert.assertTrue(transform1.hasRotation());
        Assert.assertTrue(!transform1.hasScale());
        Assert.assertTrue(position.getX() == transform1.getPosition().getX());
        Assert.assertTrue(position.getY() == transform1.getPosition().getY());
        Assert.assertTrue(position.getZ() == transform1.getPosition().getZ());
        Assert.assertTrue(rotation.getXAxis() == transform1.getRotation().getXAxis());
        Assert.assertTrue(rotation.getYAxis() == transform1.getRotation().getYAxis());
        Assert.assertTrue(rotation.getZAxis() == transform1.getRotation().getZAxis());
    }

    @Test
    public void testFoods() {
        // get lunchBuilder
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

        // generate 5 object and encode to bytes
        int cnt = 5;
        byte[] singleBytes = encoder.encode(lunchBuilder.build());
        byte[] bytes = new byte[singleBytes.length * cnt];
        for (int i = 0; i < cnt; ++i) {
            System.arraycopy(singleBytes, 0, bytes, i * singleBytes.length, singleBytes.length);
        }

        // decode
        List<Message> messageList = decoder.decode(bytes);
        Assert.assertNotNull(messageList);
        Assert.assertEquals(messageList.size(), cnt);
        for (Message message : messageList) {
            Assert.assertTrue(message instanceof FoodOuterClass.Lunch);
            FoodOuterClass.Lunch lunch = (FoodOuterClass.Lunch) message;
            Assert.assertTrue(lunch.getFoodsCount() == 4);
            List<FoodOuterClass.Food> foods = lunch.getFoodsList();

            FoodOuterClass.Food food0 = foods.get(0);
            Assert.assertTrue(food0.getFoodOneofCase() == FoodOuterClass.Food.FoodOneofCase.RICE);
            FoodOuterClass.Rice rice1 = food0.getRice();
            Assert.assertEquals(rice1.getOriginPlace(), rice.getOriginPlace());

            FoodOuterClass.Food food1 = foods.get(1);
            Assert.assertTrue(food1.getFoodOneofCase() == FoodOuterClass.Food.FoodOneofCase.NOODLE);
            FoodOuterClass.Noodle noodle1 = food1.getNoodle();
            Assert.assertEquals(noodle1.getMaterial(), noodle.getMaterial());
            Assert.assertEquals(noodle1.getTaste().getNumber(), noodle.getTaste().getNumber());
        }
    }

    @Test
    public void testErrorTotalLen() {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        byte[] bytes = encoder.encode(position);

        // total len < require size (4 + 16 + 4)
        byte[] errorBytes = ByteBuffer.allocate(4).putInt(1).array();
        System.arraycopy(errorBytes, 0, bytes, 0, errorBytes.length);
        boolean isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.LenTooSmall);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }

        // total len > readable bytes
        errorBytes = ByteBuffer.allocate(4).putInt(bytes.length + 1).array();
        System.arraycopy(errorBytes, 0, bytes, 0, errorBytes.length);
        List<Message> messageList = decoder.decode(bytes);
        Assert.assertNotNull(messageList);
        Assert.assertEquals(messageList.size(), 0);

        // total len > total len limit
        bytes = new byte[decoder.getTotalLenLimit() + 1];
        errorBytes = ByteBuffer.allocate(4).putInt(decoder.getTotalLenLimit() + 1).array();
        System.arraycopy(errorBytes, 0, bytes, 0, errorBytes.length);
        isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.LenBeyondLimit);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }
    }

    @Test
    public void testErrorNameLen() {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        byte[] bytes = encoder.encode(position);

        // name len > name len limit
        byte[] errorBytes = ByteBuffer.allocate(4).putInt(decoder.getNameLenLimit() + 1).array();
        System.arraycopy(errorBytes, 0, bytes, 4 + 16, errorBytes.length);
        boolean isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.NameLenBeyondLimit);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }

        // name <= 0
        errorBytes = ByteBuffer.allocate(4).putInt(-1).array();
        System.arraycopy(errorBytes, 0, bytes, 4 + 16, errorBytes.length);
        isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.NameLenTooSmall);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }

        // totalLen < 4 + 16 + 4 + nameLen
        errorBytes = ByteBuffer.allocate(4).putInt(bytes.length + 1).array();
        System.arraycopy(errorBytes, 0, bytes, 4 + 16, errorBytes.length);
        isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.LenTooSmall);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }
    }

    @Test
    public void testErrorParse() {
        Gameobject.Position position = Gameobject.Position.newBuilder()
                .setX(5.0f).setY(1.0f).setZ(6.0f).build();
        byte[] bytes = encoder.encode(position);

        // error protobuf name
        byte[] errorBytes = "xxx".getBytes();
        System.arraycopy(errorBytes, 0, bytes, 4 + 16 + 4, errorBytes.length);
        boolean isCatch = false;
        try {
            decoder.decode(bytes);
        } catch (Exception e) {
            Assert.assertEquals(e.getMessage(), DecodeError.FailedParseName);
            isCatch = true;
        } finally {
            Assert.assertTrue(isCatch);
        }
    }
}
