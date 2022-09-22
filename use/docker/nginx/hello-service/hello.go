package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"strings"

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
	msg := fmt.Sprintf("hello %v[%v]", name, GetClientIp(r))
	fmt.Fprint(w, msg)
}

func GetClientIp(r *http.Request) string {
	ip := ""

	// X-Forwarded-For
	xForwardedFor := r.Header.Get("X-Forwarded-For")
	ipArray := strings.Split(xForwardedFor, ",")
	if len(ipArray) > 0 {
		ip = strings.TrimSpace(ipArray[0])
		if ip != "" {
			return ip
		}
	}

	// X-Real-IP
	ip = strings.TrimSpace(r.Header.Get("X-Real-IP"))
	if ip != "" {
		return ip
	}

	if ip, _, err := net.SplitHostPort(strings.TrimSpace(r.RemoteAddr)); err == nil {
		return ip
	}

	return ""
}
