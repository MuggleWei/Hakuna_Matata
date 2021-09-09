package config

import (
	"encoding/xml"
	"fmt"
	"io"
	"os"

	log "github.com/sirupsen/logrus"
)

type MyConfig struct {
	Config Server
}

type Server struct {
	XMLName xml.Name  `xml:"server"`
	Db      DbConfig  `xml:"db"`
	Log     LogConfig `xml:"log"`
}

type LogConfig struct {
	XMLName xml.Name `xml:"log"`
	File    string   `xml:"file,attr"`
	Level   string   `xml:"level,attr"`
}

type DbConfig struct {
	XMLName xml.Name `xml:"db"`
	User    string   `xml:"user,attr"`
	Passwd  string   `xml:"passwd,attr"`
	Addr    string   `xml:"addr,attr"`
	Db      string   `xml:"db,attr"`
}

// load config file
func (this *MyConfig) Load(filepath string) error {
	fmt.Printf("start load config: filepath=%v\n", filepath)

	file, err := os.Open(filepath)
	if err != nil {
		fmt.Printf("failed open config file: filepath=%v, err=%v\n", filepath, err.Error())
		return err
	}
	defer file.Close()

	content, err := io.ReadAll(file)
	if err != nil {
		fmt.Printf("failed read file content: filepath=%v, err=%v\n", filepath, err.Error())
		return err
	}

	err = xml.Unmarshal(content, &this.Config)
	if err != nil {
		fmt.Printf("failed parse config file: filepath=%v, err=%v\n", filepath, err)
		return err
	}

	fmt.Printf("complete load config: filepath=%v\n", filepath)
	fmt.Printf("-------------------------\n")
	fmt.Printf("log config: file=%v, level=%v\n", this.Config.Log.File, this.Config.Log.Level)
	fmt.Printf("db config: user=%v, passwd=******, addr=%v, db_name=%v\n",
		this.Config.Db.User, this.Config.Db.Addr, this.Config.Db.Db)

	return nil
}

// get log level from string
func (this *MyConfig) ParseLogLevel(lvl string) (log.Level, error) {
	return log.ParseLevel(lvl)
}
