package controller

import (
	"encoding/json"
	"net/http"
	"sync"

	"example.com/hello/entity"
	"example.com/hello/model"
	"example.com/hello/service"
	log "github.com/sirupsen/logrus"
)

type AlbumController struct {
	albumService *service.AlbumService
}

var (
	singletonAlbum *AlbumController
	onceAlbum      sync.Once
)

func GetAlbumController() *AlbumController {
	if singletonAlbum == nil {
		onceAlbum.Do(func() {
			singletonAlbum = &AlbumController{
				albumService: service.GetAlbumService(),
			}
		})
	}
	return singletonAlbum
}

// convert album entity to models
func (this *AlbumController) ConvertEntityToModel(albums []entity.AlbumEntity) []*model.AlbumModel {
	albumModels := make([]*model.AlbumModel, 0)
	for _, v := range albums {
		am := &model.AlbumModel{
			Title:  v.Title,
			Artist: v.Artist,
			Price:  v.Price,
		}
		albumModels = append(albumModels, am)
	}

	return albumModels
}

func (this *AlbumController) ResponseEntity(w http.ResponseWriter, albums []entity.AlbumEntity) {
	// convert entity to model
	albumsModels := this.ConvertEntityToModel(albums)

	// response
	res := model.Response{}
	res.Code = 0
	res.Data = albumsModels
	HttpResponse(w, &res)
}

// query by title and artist
func (this *AlbumController) Query(w http.ResponseWriter, r *http.Request) {
	// get query parameters
	query := r.URL.Query()
	title := query.Get("title")
	artist := query.Get("artist")

	// query albums
	var err error
	var albums []entity.AlbumEntity
	if title != "" || artist != "" {
		albums, err = this.albumService.FindAlbum(title, artist)
		if err != nil {
			log.Warning("failed find album: err=%v", err)
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
	} else {
		log.Warning("query album without title and artist")
		http.Error(w, "invalid params", http.StatusBadRequest)
		return
	}

	// response
	this.ResponseEntity(w, albums)
}

// query by price range
func (this *AlbumController) QueryByPrice(w http.ResponseWriter, r *http.Request) {
	var qry model.QueryByPriceModel
	err := json.NewDecoder(r.Body).Decode(&qry)
	if err != nil {
		log.Warning("failed parse body: %v", err.Error())
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	// query albums
	var albums []entity.AlbumEntity
	albums, err = this.albumService.FindAlbumByPriceRange(qry.PriceLower, qry.PriceUpper)
	if err != nil {
		log.Warning("failed find album by price: err=%v", err)
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// response
	this.ResponseEntity(w, albums)
}
