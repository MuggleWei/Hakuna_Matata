package main

import (
	"fmt"

	"github.com/spf13/pflag"
	"github.com/spf13/viper"
)

func printValues() {
	host := viper.GetString("host")
	port := viper.GetUint("port")
	lang := viper.GetStringMapString("lang")

	fmt.Printf("host: %v, port: %v\n", host, port)
	fmt.Printf("lang:\n")
	for k, v := range lang {
		fmt.Printf("\t%v: %v\n", k, v)
	}
}

func main() {
	// set default value
	viper.SetDefault("host", "127.0.0.1")
	viper.SetDefault("port", 8080)
	viper.SetDefault("lang", map[string]string{
		"c":      "yes",
		"c++":    "aha",
		"python": "yes",
		"go":     "yes",
		"java":   "emm...",
		"c#":     "good",
		"rust":   "maybe",
		"js":     "annoying",
	})

	// read config files
	fmt.Println("-----------------------")
	fmt.Println("read config")
	viper.SetConfigName("config")
	viper.SetConfigType("yaml")
	viper.AddConfigPath(".")
	viper.AddConfigPath("./config")
	err := viper.ReadInConfig()
	if err != nil {
		_, ok := err.(viper.ConfigFileNotFoundError)
		if ok {
			fmt.Printf("config file not found\n")
		} else {
			panic(fmt.Errorf("fatal error config file: %v", err))
		}
	} else {
		printValues()
	}

	// read env
	fmt.Println("-----------------------")
	fmt.Println("read env")
	viper.SetEnvPrefix("viper") // will be uppercased automatically
	viper.BindEnv("host")
	viper.BindEnv("port")
	printValues()

	// read flags
	fmt.Println("-----------------------")
	fmt.Println("read flag")
	pflag.String("host", "10.0.2.5", "bind host")
	pflag.Uint("port", 0, "listen port")
	pflag.Parse()
	viper.BindPFlags(pflag.CommandLine)
	printValues()
}
