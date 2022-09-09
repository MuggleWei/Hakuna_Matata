package main

import (
	"flag"
	"log"
	"net/http"
)

func main() {
	host := flag.String("host", "127.0.0.1", "server host")
	port := flag.String("port", "8080", "server port")
	directory := flag.String("dir", ".", "root directory for files")
	flag.Parse()

	addr := *host + ":" + *port

	http.Handle("/", http.FileServer(http.Dir(*directory)))

	log.Printf("Run Serving http fileserver, addr: %v, root dirctory: %v\n", addr, *directory)
	log.Fatal(http.ListenAndServe(addr, nil))
}
