package main

import (
	"errors"
	"flag"
	"fmt"
	"net/http"
	"os"
	"time"

	"example.com/hello/config"
	"example.com/hello/controller"
	"example.com/hello/db"
	"github.com/go-sql-driver/mysql"
	"github.com/gorilla/mux"
	log "github.com/sirupsen/logrus"
	"gopkg.in/natefinch/lumberjack.v2"
)

func initLog(cfg *config.MyConfig) error {
	fmt.Println("start initialize log")

	lvl, err := cfg.ParseLogLevel(cfg.Config.Log.Level)
	if err != nil {
		fmt.Printf("invalid log level: %v\n", cfg.Config.Log.Level)
		return errors.New("invalid log leve")
	}

	if cfg.Config.Log.File == "" {
		log.SetOutput(os.Stdout)
	} else {
		log.SetOutput(&lumberjack.Logger{
			Filename:   cfg.Config.Log.File,
			MaxSize:    128, // megabytes
			MaxBackups: 3,
			MaxAge:     28,   //days
			Compress:   true, // disabled by default
		})
	}
	log.SetReportCaller(true)
	log.SetFormatter(&config.MyLogFormat{})
	log.SetLevel(lvl)

	log.Info("success initialize log")

	return nil
}

func initDB(cfg *config.MyConfig) error {
	log.Info("start initialize db")

	dbMap := make(map[string]string)
	for _, db := range cfg.Config.DataSources.Dbs {
		mysqlCfg := mysql.Config{
			User:                 db.User,
			Passwd:               db.Passwd,
			Net:                  "tcp",
			Addr:                 db.Addr,
			DBName:               db.Db,
			AllowNativePasswords: true,
		}
		dbMap[db.Name] = mysqlCfg.FormatDSN()
	}

	err := db.InitInstance(dbMap)
	if err != nil {
		log.Errorf("failed init db instance: err=%v", err)
		return err
	}

	log.Info("success initialize db")

	return nil
}

func main() {
	// init flags
	configFilePath := flag.String("f", "conf/cfg.xml", "config file path")
	flag.Parse()

	// load config file
	cfg := config.MyConfig{}
	err := cfg.Load(*configFilePath)
	if err != nil {
		fmt.Println("failed load config file, exit...")
		os.Exit(1)
	}

	// initialize log
	err = initLog(&cfg)
	if err != nil {
		fmt.Printf("failed initialize log: err=%v\n", err)
		os.Exit(1)
	}

	// initialize db
	err = initDB(&cfg)
	if err != nil {
		log.Errorf("failed initialize db: err=%v", err)
		os.Exit(1)
	}

	// http server
	router := mux.NewRouter()

	helloController := controller.GetHelloController()
	helloRouter := router.PathPrefix("/api/v1/hello").Subrouter()
	helloRouter.HandleFunc("/echo", helloController.Echo)
	helloRouter.HandleFunc("/greet", helloController.Greet)

	albumController := controller.GetAlbumController()
	albumRouter := router.PathPrefix("/api/v1/album").Subrouter()
	albumRouter.HandleFunc("/query", albumController.Query).Methods("GET")
	albumRouter.HandleFunc("/query_by_price", albumController.QueryByPrice).Methods("POST")

	router.Use(controller.MiddlewareTimeElapsed)

	srv := &http.Server{
		Handler:      router,
		Addr:         cfg.Config.Http.Addr,
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}
