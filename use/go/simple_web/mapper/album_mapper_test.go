package mapper

import (
	"fmt"
	"os"
	"testing"

	"example.com/hello/config"
	"example.com/hello/db"
	"github.com/go-sql-driver/mysql"
	log "github.com/sirupsen/logrus"

	"github.com/stretchr/testify/assert"
)

func init() {
	cfg := config.MyConfig{}
	err := cfg.Load("../conf/cfg_dev.xml")
	if err != nil {
		fmt.Println("failed load config file, exit...")
		os.Exit(1)
	}

	mysqlCfg := mysql.Config{
		User:                 cfg.Config.Db.User,
		Passwd:               cfg.Config.Db.Passwd,
		Net:                  "tcp",
		Addr:                 cfg.Config.Db.Addr,
		DBName:               cfg.Config.Db.Db,
		AllowNativePasswords: true,
	}

	err = db.InitInstance(mysqlCfg.FormatDSN())
	if err != nil {
		log.Errorf("failed init db instance: err=%v", err)
		os.Exit(1)
	}
}

func TestQueryAll(t *testing.T) {
	title := ""
	artist := ""
	albums, err := GetAlbumMapper().Query(title, artist)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 4, "query albums should return 4 items")
}

func TestQueryEmpty(t *testing.T) {
	title := "xxx"
	artist := "xxx"

	albums, err := GetAlbumMapper().Query(title, artist)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 0, "query albums should return 0 items")
}

func TestQueryByTitle(t *testing.T) {
	title := "Jeru"
	artist := ""
	albums, err := GetAlbumMapper().Query(title, artist)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 1, "query albums should return 4 items")
	assert.Equal(t, albums[0].Title, title, "title not equal")
	assert.Equal(t, albums[0].Artist, "Gerry Mulligan", "title not equal")
	assert.Equal(t, albums[0].Price, 17.99, "price not equal")
}

func TestQueryByArtist(t *testing.T) {
	title := ""
	artist := "John Coltrane"
	albums, err := GetAlbumMapper().Query(title, artist)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 2, "query albums should return 2 items")
	for _, v := range albums {
		assert.Equal(t, v.Artist, artist, "artist not equal")
	}
}

func TestQueryByTitleAndArtist(t *testing.T) {
	title := "Blue Train"
	artist := "John Coltrane"
	albums, err := GetAlbumMapper().Query(title, artist)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 1, "query albums should return 1 items")
	assert.Equal(t, albums[0].Title, title, "title not equal")
	assert.Equal(t, albums[0].Artist, artist, "artist not equal")
}

func TestQueryByPriceRange(t *testing.T) {
	priceLower := 50.0
	priceUpper := 100.0
	albums, err := GetAlbumMapper().QueryByPriceRange(priceLower, priceUpper)

	assert.Nil(t, err, "query albums should return nil error")
	assert.Equal(t, len(albums), 2, "query albums should return 2 items")
	for _, v := range albums {
		assert.GreaterOrEqual(t, v.Price, priceLower, "price beyond the lower")
		assert.LessOrEqual(t, v.Price, priceUpper, "price beyond the upper")
	}
}
