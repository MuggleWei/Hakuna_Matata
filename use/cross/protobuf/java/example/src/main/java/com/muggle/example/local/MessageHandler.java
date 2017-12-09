package com.muggle.example.local;

import com.google.protobuf.Message;
import com.muggle.proto.handler.ByteMessageHandler;
import com.muggle.proto.handler.ByteProtoCallback;
import gen.proto.FoodOuterClass;
import gen.proto.Gameobject;

public class MessageHandler extends ByteMessageHandler {
    public MessageHandler(String descFileName) {
        super(descFileName);
    }

    public void Registers() {
        RegisterDefault(new DefaultCallback());

        Register(Gameobject.Position.newBuilder().build(), new PositionCallback());
        Register(Gameobject.Rotation.newBuilder().build(), new RotationCallback());
        Register(Gameobject.Transform.newBuilder().build(), new TransformCallback());
        Register(FoodOuterClass.Lunch.newBuilder().build(), new LunchCallback());
    }

    class DefaultCallback implements ByteProtoCallback {
        @Override
        public void OnMessage(Message msg) {
            System.out.println("========");
            System.out.println("on message: " + msg.getDescriptorForType().getFullName());
        }
    }

    class PositionCallback implements ByteProtoCallback {
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

    class RotationCallback implements ByteProtoCallback {

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

    class TransformCallback implements ByteProtoCallback {

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

    class LunchCallback implements ByteProtoCallback {

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
}
