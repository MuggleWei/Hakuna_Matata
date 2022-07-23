package controller

import (
	"net/http"
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
