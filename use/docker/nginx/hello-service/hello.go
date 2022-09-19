package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"

	"github.com/gorilla/mux"
)

func init() {
	log.SetOutput(os.Stdout)
	log.SetFlags(log.LstdFlags | log.LUTC | log.Lmicroseconds | log.Lshortfile)
}

func main() {
	bindAddr := flag.String("bind", ":10102", "bind address")
	number := flag.String("number", "0", "number")
	flag.Parse()

	router := mux.NewRouter().StrictSlash(true)
	router.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprint(w, "Welcome! it's "+*number)
	})
	router.HandleFunc("/hello", HelloIndex)
	router.HandleFunc("/hello/{name}", Hello)

	http.ListenAndServe(*bindAddr, router)
}

func HelloIndex(w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, "Hello stranger")
}

func Hello(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	name := vars["name"]
	fmt.Fprint(w, "hello ", name)
}
