package com.muggle.helloproto;

import com.google.protobuf.Message;
import com.muggle.proto.ProtoCallback;
import com.muggle.proto.Receiver;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;

public class MyReceiver extends Receiver {
    public void Register() {
        messageHandler.RegisterDefault(new DefaultCallback());

        messageHandler.Register(Gameobject.Position.newBuilder().build(), new PositionCallback());
        messageHandler.Register(Gameobject.Rotation.newBuilder().build(), new RotationCallback());
        messageHandler.Register(Gameobject.Transform.newBuilder().build(), new TransformCallback());
        messageHandler.Register(FoodOuterClass.Lunch.newBuilder().build(), new LunchCallback());
    }
}

class DefaultCallback implements ProtoCallback {
    @Override
    public void OnMessage(Message msg) {
        System.out.println("========");
        System.out.println("on message: " + msg.getDescriptorForType().getFullName());
    }
}

class PositionCallback implements ProtoCallback {
    @Override
    public void OnMessage(Message msg) {
        Gameobject.Position pos = (Gameobject.Position) msg;
        System.out.println("========");
        System.out.println("OnMessagePosition: \n"
                + pos.getX() + ", "
                + pos.getX() + ", "
                + pos.getX());
    }
}

class RotationCallback implements ProtoCallback {

    @Override
    public void OnMessage(Message msg) {
        Gameobject.Rotation rot = (Gameobject.Rotation) msg;
        System.out.println("========");
        System.out.println("OnMessageRotation: \n"
                + rot.getXAxis() + ", "
                + rot.getYAxis() + ", "
                + rot.getZAxis()
        );
    }
}

class TransformCallback implements ProtoCallback {

    @Override
    public void OnMessage(Message msg) {
        Gameobject.Transform trans = (Gameobject.Transform) msg;
        System.out.println("========");
        System.out.println("OnMessageTransform: \n"
                + trans.getPosition().getX() + ", " + trans.getPosition().getY() + ", " + trans.getPosition().getZ() + "\n"
                + trans.getRotation().getXAxis() + ", " + trans.getRotation().getYAxis() + ", " + trans.getRotation().getZAxis() + "\n"
                + trans.getScale().getXScale() + ", " + trans.getScale().getYScale() + ", " + trans.getScale().getZScale()
        );
    }
}

class LunchCallback implements ProtoCallback {

    @Override
    public void OnMessage(Message msg) {
        FoodOuterClass.Lunch lunch = (FoodOuterClass.Lunch) msg;
        System.out.println("========");
        System.out.println("OnMessageLunch:");
        for (FoodOuterClass.Food food : lunch.getFoodsList()) {
            switch (food.getFoodOneofCase()) {
                case RICE: {
                    System.out.println("rice: " + food.getRice().getOriginPlace());
                }
                break;
                case DUMPLING: {
                    System.out.println("dumpling: " + food.getDumpling().getStuffing() + ", " + food.getDumpling().getTaste());
                }
                break;
                case NOODLE: {
                    System.out.println("noodle: " + food.getNoodle().getMaterial() + ", " + food.getNoodle().getTaste());
                }
                break;
                case FOODONEOF_NOT_SET: {
                    System.out.println("no set");
                }
            }
        }

    }
}
