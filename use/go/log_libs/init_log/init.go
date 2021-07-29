package main

import (
	"bytes"
	"fmt"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"time"

	"github.com/sirupsen/logrus"
	"gopkg.in/natefinch/lumberjack.v2"
)

type MyLogFormat struct{}

var log = logrus.New()

func init() {
	log.SetOutput(&lumberjack.Logger{
		Filename:   "./log/hello.log",
		MaxSize:    500, // megabytes
		MaxBackups: 3,
		MaxAge:     28,   //days
		Compress:   true, // disabled by default
	})
	log.SetLevel(logrus.InfoLevel)
	log.SetFormatter(&MyLogFormat{})
	log.SetReportCaller(true)
}

func (m *MyLogFormat) Format(entry *logrus.Entry) ([]byte, error) {
	var b *bytes.Buffer
	if entry.Buffer != nil {
		b = entry.Buffer
	} else {
		b = &bytes.Buffer{}
	}

	timestamp := entry.Time.Format(time.RFC3339)
	goid := m.Goid()

	var newLog string
	if entry.HasCaller() {
		fName := filepath.Base(entry.Caller.File)
		newLog = fmt.Sprintf("%s|%s|%d|%s:%d %s - %s\n",
			entry.Level, timestamp, goid, fName, entry.Caller.Line, entry.Caller.Function, entry.Message)
	} else {
		newLog = fmt.Sprintf("%s|%s - %s\n", timestamp, entry.Level, entry.Message)
	}

	b.WriteString(newLog)
	return b.Bytes(), nil
}

func (m *MyLogFormat) Goid() int {
	var buf [64]byte
	n := runtime.Stack(buf[:], false)
	idField := strings.Fields(strings.TrimPrefix(string(buf[:n]), "goroutine "))[0]
	id, err := strconv.Atoi(idField)
	if err != nil {
		//panic(fmt.Sprintf("cannot get goroutine id: %v", err))
		return 0
	}
	return id
}
