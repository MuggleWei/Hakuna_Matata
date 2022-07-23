package service

import (
	"testing"
	"time"

	"github.com/stretchr/testify/assert"
)

func TestGetYMD(t *testing.T) {
	dateUtils := GetDateUtils()

	var dateTime time.Time
	var date int

	dateTime = time.Date(2021, 9, 10, 0, 0, 0, 0, dateUtils.Loc)
	date = dateUtils.GetYMD(&dateTime)
	assert.Equal(t, date, 20210910, "")

	dateTime = time.Date(2021, 9, 10, 8, 0, 0, 0, dateUtils.Loc)
	date = dateUtils.GetYMD(&dateTime)
	assert.Equal(t, date, 20210910, "")

	dateTime = time.Date(2021, 9, 10, 10, 0, 0, 0, dateUtils.Loc)
	date = dateUtils.GetYMD(&dateTime)
	assert.Equal(t, date, 20210910, "")
}

func TestCalcDate(t *testing.T) {
	var date int

	date = GetDateUtils().CalcDate(20210910, -1, 0, 0)
	assert.Equal(t, date, 20200910, "")

	date = GetDateUtils().CalcDate(20210910, 0, -1, 0)
	assert.Equal(t, date, 20210810, "")

	date = GetDateUtils().CalcDate(20210910, 0, 0, -1)
	assert.Equal(t, date, 20210909, "")
}

func TestCalcNatureYear(t *testing.T) {
	var date int
	var deltaYear int

	date = 20210910
	deltaYear = 0
	date = GetDateUtils().CalcNatureYear(date, deltaYear)
	assert.Equal(t, date, 20210101, "")

	date = 20210910
	deltaYear = -1
	date = GetDateUtils().CalcNatureYear(date, deltaYear)
	assert.Equal(t, date, 20200101, "")

	date = 20210910
	deltaYear = -2
	date = GetDateUtils().CalcNatureYear(date, deltaYear)
	assert.Equal(t, date, 20190101, "")
}

func TestCalcNatureMonth(t *testing.T) {
	var date int
	var deltaMonth int

	date = 20210910
	deltaMonth = 0
	date = GetDateUtils().CalcNatureMonth(date, deltaMonth)
	assert.Equal(t, date, 20210901, "")

	date = 20210910
	deltaMonth = -1
	date = GetDateUtils().CalcNatureMonth(date, deltaMonth)
	assert.Equal(t, date, 20210801, "")

	date = 20210910
	deltaMonth = -24
	date = GetDateUtils().CalcNatureMonth(date, deltaMonth)
	assert.Equal(t, date, 20190901, "")
}

func TestFirstDayOfWeek(t *testing.T) {
	var date int

	date = 20210910
	date = GetDateUtils().FirstDayOfWeek(date)
	assert.Equal(t, date, 20210906)

	date = 20210901
	date = GetDateUtils().FirstDayOfWeek(date)
	assert.Equal(t, date, 20210830)

	date = 20220101
	date = GetDateUtils().FirstDayOfWeek(date)
	assert.Equal(t, date, 20211227)
}

func TestCalcNatureWeek(t *testing.T) {
	var date int
	var deltaWeek int

	date = 20210910
	deltaWeek = 0
	date = GetDateUtils().CalcNatureWeek(date, deltaWeek)
	assert.Equal(t, date, 20210906)

	date = 20210910
	deltaWeek = -1
	date = GetDateUtils().CalcNatureWeek(date, deltaWeek)
	assert.Equal(t, date, 20210830)
}
