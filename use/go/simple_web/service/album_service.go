package service

import (
	"sync"

	"example.com/hello/entity"
	"example.com/hello/mapper"
)

type AlbumService struct {
	albumMapper *mapper.AlbumMapper
}

var (
	singleton *AlbumService
	once      sync.Once
)

func GetAlbumService() *AlbumService {
	if singleton == nil {
		once.Do(func() {
			singleton = &AlbumService{
				albumMapper: mapper.GetAlbumMapper(),
			}
		})
	}
	return singleton
}

// find album by title and artist
func (this *AlbumService) FindAlbum(title, artist string) ([]entity.AlbumEntity, error) {
	return this.albumMapper.Query(title, artist)
}
