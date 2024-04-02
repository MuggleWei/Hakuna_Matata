package controller

import (
	"compress/gzip"
	"io"
	"net/http"
	"strings"
	"time"

	log "github.com/sirupsen/logrus"
)

func MiddlewareTimeElapsed(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		startTime := time.Now()

		next.ServeHTTP(w, r)

		elapsedTime := time.Since(startTime)
		log.Infof("url=%v, params=%v, elapsed=%v",
			r.URL.Path, r.URL.RawQuery, elapsedTime)
	})
}

type gzipResponseWriter struct {
	io.Writer
	http.ResponseWriter
}

func (w gzipResponseWriter) Write(b []byte) (int, error) {
	return w.Writer.Write(b)
}

func MiddlewareGZip(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if strings.Contains(r.Header.Get("Accept-Encoding"), "gzip") {
			gzipWriter := gzip.NewWriter(w)
			defer gzipWriter.Close()

			w.Header().Set("Content-Encoding", "gzip")

			next.ServeHTTP(
				gzipResponseWriter{Writer: gzipWriter, ResponseWriter: w}, r)

			log.Infof("url=%v, params=%v, use gzip",
				r.URL.Parse, r.URL.RawQuery)
		} else {
			next.ServeHTTP(w, r)
		}
	})
}
