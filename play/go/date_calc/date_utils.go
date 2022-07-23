package service

import (
	"math"
	"sync"
	"time"
)

type DateUtils struct {
	Loc *time.Location
}

var (
	singleton *DateUtils
	once      sync.Once
)

func GetDateUtils() *DateUtils {
	if singleton == nil {
		once.Do(func() {
			loc := time.UTC
			//loc, err := time.LoadLocation("Asia/Shanghai")
			//if err != nil {
			//    log.Error("failed load location Asia/Shanghai")
			//    loc = time.Local
			//}
			singleton = &DateUtils{
				Loc: loc,
			}
		})
	}
	return singleton
}

// get integer yyyymmdd format of date
func (this *DateUtils) GetYMD(date *time.Time) int {
	return date.Year()*10000 + ((int)(date.Month()))*100 + date.Day()
}

// get date
func (this *DateUtils) GetDate(date int) time.Time {
	var dateTime time.Time
	if date <= 0 {
		dateTime = time.Now().In(this.Loc)
	} else {
		y := date / 10000
		m := (time.Month)((date % 10000) / 100)
		d := date % 100
		dateTime = time.Date(y, m, d, 0, 0, 0, 0, this.Loc)
	}

	return dateTime
}

// get date = curDate + deltaYear + deltaMonth + deltaDay
func (this *DateUtils) CalcDate(date, deltaYear, deltaMonth, deltaDay int) int {
	dateTime := this.GetDate(date)
	retDate := dateTime.AddDate(deltaYear, deltaMonth, deltaDay)
	return this.GetYMD(&retDate)
}

// get nature year date
// e.g.
//     date=20210910, deltaYear=0, result=202110101
//     date=20210910, deltaYear=-1, result=202010101
func (this *DateUtils) CalcNatureYear(date, deltaYear int) int {
	if date <= 0 {
		dateTime := time.Now().In(this.Loc)
		date = this.GetYMD(&dateTime)
	}

	y := date / 10000
	y += deltaYear
	return y*10000 + 1*100 + 1
}

// get nature month date
// e.g.
//     date=20210910, deltaMonth=0, result=20210901
//     date=20210910, deltaMonth=-1, result=20210801
func (this *DateUtils) CalcNatureMonth(date, deltaMonth int) int {
	if date <= 0 {
		dateTime := time.Now().In(this.Loc)
		date = this.GetYMD(&dateTime)
	}

	y := date / 10000
	m := (date % 10000) / 100

	deltaYear := 0
	if math.Abs(float64(deltaMonth)) > 12 {
		deltaYear += deltaMonth / 12
		deltaMonth = deltaMonth % 12
	}

	y += deltaYear
	m += deltaMonth

	return y*10000 + m*100 + 1
}

// get first day of week
func (this *DateUtils) FirstDayOfWeek(date int) int {
	dateTime := this.GetDate(date)
	weekDay := dateTime.Weekday()

	deltaDay := 0
	switch weekDay {
	case time.Sunday:
		deltaDay = -6
	case time.Monday:
		deltaDay = 0
	case time.Tuesday:
		deltaDay = -1
	case time.Wednesday:
		deltaDay = -2
	case time.Thursday:
		deltaDay = -3
	case time.Friday:
		deltaDay = -4
	case time.Saturday:
		deltaDay = -5
	}

	retDate := dateTime.AddDate(0, 0, deltaDay)
	return this.GetYMD(&retDate)
}

// get nature week date
// e.g.
//     date=20210910, deltaWeek=0, result=20210906
//     date=20210910, deltaWeek=-1, result=20210830
func (this *DateUtils) CalcNatureWeek(date, deltaWeek int) int {
	date = this.FirstDayOfWeek(date)
	if deltaWeek != 0 {
		date = this.CalcDate(date, 0, 0, deltaWeek*7)
	}
	return date
}
