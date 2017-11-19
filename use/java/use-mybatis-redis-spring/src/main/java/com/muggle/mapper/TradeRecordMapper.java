package com.muggle.mapper;

import com.muggle.model.TradeRecord;
import org.apache.ibatis.annotations.Select;
import org.apache.ibatis.annotations.Update;

import java.util.List;

public interface TradeRecordMapper {

    @Update({"insert into trade_record (id, account_id, product_id, price, trade_time, trade_date) " +
            "value(#{id}, #{account_id}, #{product_id}, #{price}, #{trade_time}, #{trade_date})"})
    int InsertRecord(TradeRecord record) throws Exception;

    @Select({"select * from trade_record where account_id = #{account_id}"})
    List<TradeRecord> selectRecordByAccountId(long id);

}
