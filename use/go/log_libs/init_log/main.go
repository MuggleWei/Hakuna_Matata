package main

func main() {
	for i := 0; i < 1024*256; i++ {
		log.Trace("trace message")
		log.Debug("debug message")
		log.Info("info message")
		log.Warn("warning message")
		log.Error("error message")
		//log.Fatal("fatal")
	}
}
