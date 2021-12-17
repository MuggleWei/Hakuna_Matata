package controller

import (
	"encoding/json"
	"net/http"

	"example.com/hello/model"
	log "github.com/sirupsen/logrus"
)

func HttpResponse(w http.ResponseWriter, res *model.Response) {
	b, err := json.Marshal(*res)
	if err != nil {
		log.Warning("failed marshal response model")
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.Write(b)
}
