package main

import (
	"fmt"
	"net"
	"time"

	codec "example.mugglewei.com/use_tcp/codec"
)

func main() {
	conn, err := net.Dial("tcp", ":8080")
	if err != nil {
		panic(err)
	}
	fmt.Println("success connect")

	defer conn.Close()

	bytesCodec := codec.BytesCodec{
		MaxPayloadLimit: 4 * 1024,
	}
	for i := 0; i < 20; i++ {
		msg := "hello world"
		payload := []byte(msg)
		b, err := bytesCodec.Encode(1, payload)
		if err != nil {
			panic(err)
		}

		// test
		//conn.Write(b[:8])
		//time.Sleep(5000 * time.Millisecond)
		//conn.Write(b[8:17])
		//time.Sleep(5000 * time.Millisecond)
		//conn.Write(b[17:])

		conn.Write(b)

		fmt.Printf("send msg: %v\n", msg)
		time.Sleep(500 * time.Millisecond)
	}
	fmt.Println("bye")
}
