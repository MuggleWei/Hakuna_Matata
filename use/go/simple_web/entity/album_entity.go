package entity

type AlbumEntity struct {
	Id     int64
	Title  string
	Artist string
	Price  float64
}

func (this *AlbumEntity) TableName() string {
	return "album"
}
