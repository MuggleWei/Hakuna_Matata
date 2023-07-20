# Pandas 简单用法

## concat_column
* 模拟获取股票行情, 以时间戳为 index
* 获取所有的时间戳, 并在每个 DataFrame 中填充时间戳
* 通过前一日的收盘价填充 NA
* 将剩余的 NA 填充为 0.0
* 将所有股票的收盘价合并到同一张 DataFrame 当中

## add_column
* 模拟获取股票行情, 以时间戳为 index
* 根据 close 列, 计算 MA5 列
