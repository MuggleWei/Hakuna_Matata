package mapper

import (
	"sync"

	"example.com/hello/db"
	"example.com/hello/entity"

	log "github.com/sirupsen/logrus"
)

type AlbumMapper struct {
	DefaultCols []string
}

var (
	singleton *AlbumMapper
	once      sync.Once
)

func GetAlbumMapper() *AlbumMapper {
	if singleton == nil {
		once.Do(func() {
			singleton = &AlbumMapper{
				DefaultCols: []string{"id", "title", "artist", "price"},
			}
		})
	}
	return singleton
}

// query album by title and artist
func (this *AlbumMapper) Query(title, artist string) ([]entity.AlbumEntity, error) {
	var albums []entity.AlbumEntity

	engine, err := db.GetInstance("hello")
	if err != nil {
		log.Error("failed get db instance 'hello'")
		return nil, err
	}

	chain := engine.Cols(this.DefaultCols...).Where("")
	if title != "" {
		chain = chain.And("title=?", title)
	}
	if artist != "" {
		chain = chain.And("artist=?", artist)
	}
	err = chain.Find(&albums)
	if err != nil {
		log.Errorf("failed query album: title=%v, artist=%v", title, artist)
		return nil, err
	}

	return albums, nil
}

// query album by price range
func (this *AlbumMapper) QueryByPriceRange(lower, upper float64) ([]entity.AlbumEntity, error) {
	var albums []entity.AlbumEntity

	engine, err := db.GetInstance("hello")
	if err != nil {
		log.Error("failed get db instance 'hello'")
		return nil, err
	}

	chain := engine.Cols(this.DefaultCols...).Where("price >= ? AND price <= ?", lower, upper)
	err = chain.Find(&albums)
	if err != nil {
		log.Errorf("failed query album by price range: lower=%v, upper=%v", lower, upper)
		return nil, err
	}

	return albums, nil
}
