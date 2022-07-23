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
	XMLName     xml.Name          `xml:"server"`
	Log         LogConfig         `xml:"log"`
	DataSources DataSourcesConfig `xml:"datasources"`
	Http        HttpConfig        `xml:"http"`
}

type LogConfig struct {
	XMLName xml.Name `xml:"log"`
	File    string   `xml:"file,attr"`
	Level   string   `xml:"level,attr"`
}

type DataSourcesConfig struct {
	XMLName xml.Name   `xml:"datasources"`
	Dbs     []DbConfig `xml:"db"`
}

type DbConfig struct {
	XMLName xml.Name `xml:"db"`
	Name    string   `xml:"name,attr"`
	User    string   `xml:"user,attr"`
	Passwd  string   `xml:"passwd,attr"`
	Addr    string   `xml:"addr,attr"`
	Db      string   `xml:"db,attr"`
}

type HttpConfig struct {
	XMLName xml.Name `xml:"http"`
	Addr    string   `xml:"addr,attr"`
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
	for _, db := range this.Config.DataSources.Dbs {
		fmt.Printf("db config: name=%v, user=%v, passwd=******, addr=%v, database=%v\n",
			db.Name, db.User, db.Addr, db.Db)
	}
	fmt.Printf("http config: addr:%v\n", this.Config.Http.Addr)

	return nil
}

// get log level from string
func (this *MyConfig) ParseLogLevel(lvl string) (log.Level, error) {
	return log.ParseLevel(lvl)
}
