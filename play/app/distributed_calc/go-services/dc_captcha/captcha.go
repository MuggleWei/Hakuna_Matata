package main

import (
	"log"
	"net/http"
	"strconv"

	"github.com/gorilla/mux"
	lumberjack "gopkg.in/natefinch/lumberjack.v2"
)

func init() {
	log.SetOutput(&lumberjack.Logger{
		Filename:   "./log/dc-captcha-service.log",
		MaxSize:    100,   // MB
		MaxBackups: 30,    // old files
		MaxAge:     30,    // day
		Compress:   false, // disabled by default
	})
	// log.SetOutput(os.Stdout)
	log.SetFlags(log.LstdFlags | log.LUTC | log.Lmicroseconds | log.Lshortfile)
}

func main() {
	// load config file
	config, err := LoadCaptchaConfig()
	if err != nil {
		log.Println("[err] failed load captcha config")
		panic(err)
	}
	log.Printf("%+v\n", *config)
	log.Println("load config completed")

	// init componenets
	components, err := initComponents(config)
	if err != nil {
		log.Println("[err] failed init captcha config")
		panic(err)
	}
	log.Println("init components completed")

	// rest handle
	restHandle := &CaptchaRestHandle{
		Components: components,
		Config:     config,
	}

	// restful api
	router := mux.NewRouter()
	router.HandleFunc("/captcha/load", restHandle.Load)
	router.HandleFunc("/captcha/verify", restHandle.Verify)

	addr := config.Host + ":" + strconv.Itoa(config.Port)
	log.Printf("listen address: %v\n", addr)
	err = http.ListenAndServe(addr, router)
	if err != nil {
		log.Printf("[err] failed listen at %v, err: %v\n", addr, err)
	}
}
