package controller

import (
	"encoding/json"
	"fmt"
	"net/http"
	"sync"

	"example.com/hello/entity"
	"example.com/hello/model"
	"example.com/hello/service"
)

type AlbumController struct {
	albumService *service.AlbumService
}

var (
	singleton *AlbumController
	once      sync.Once
)

func GetAlbumController() *AlbumController {
	if singleton == nil {
		once.Do(func() {
			singleton = &AlbumController{
				albumService: service.GetAlbumService(),
			}
		})
	}
	return singleton
}

func (this *AlbumController) Query(w http.ResponseWriter, r *http.Request) {
	query := r.URL.Query()
	title := query.Get("title")
	artist := query.Get("artist")

	var err error
	var albums []entity.AlbumEntity
	if title != "" || artist != "" {
		albums, err = this.albumService.FindAlbum(title, artist)
	} else {
		http.Error(w, "invalid params", http.StatusForbidden)
		return
	}

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// convert entity to model
	albumModels := make([]*model.AlbumModel, 0)
	for _, v := range albums {
		am := &model.AlbumModel{
			Title:  v.Title,
			Artist: v.Artist,
			Price:  v.Price,
		}
		albumModels = append(albumModels, am)
	}

	b, err := json.Marshal(albumModels)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	fmt.Fprintf(w, string(b))
}
