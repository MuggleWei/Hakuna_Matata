package model

type AlbumModel struct {
	Title  string  `json:"title"`
	Artist string  `json:"artist"`
	Price  float64 `json:"price"`
}

type QueryByPriceModel struct {
	PriceLower float64 `json:"lower"`
	PriceUpper float64 `json:"upper"`
}
