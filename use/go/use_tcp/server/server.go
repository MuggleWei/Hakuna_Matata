package main

import (
	"bufio"
	"fmt"
	"net"

	codec "example.mugglewei.com/use_tcp/codec"
)

func main() {
	listen, err := net.Listen("tcp", ":8080")
	if err != nil {
		panic(err)
	}
	fmt.Println("success listen")

	for {
		conn, err := listen.Accept()
		if err != nil {
			fmt.Printf("accept failed, err:%v\n", err)
		}

		go handleConn(conn)
	}
}

func handleConn(conn net.Conn) {
	defer conn.Close()
	bytesCodec := codec.BytesCodec{
		MaxPayloadLimit: 4 * 1024,
	}
	reader := bufio.NewReader(conn)
	for {
		hdr, payload, err := bytesCodec.Decode(reader)
		if err != nil {
			fmt.Printf("decode err: %v\n", err.Error())
			break
		}
		fmt.Printf("recv msg\nhdr: %+v\npayload: %+v\n",
			hdr, payload)
	}

	fmt.Println("disconnect")
}
