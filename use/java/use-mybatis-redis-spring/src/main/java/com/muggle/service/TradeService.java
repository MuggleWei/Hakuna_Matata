package com.muggle.service;

import com.muggle.mapper.AccountMapper;
import com.muggle.mapper.TradeRecordMapper;
import com.muggle.model.TradeRecord;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.data.redis.core.HashOperations;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.List;

@Service
@EnableCaching
public class TradeService {
    @Autowired
    private AccountMapper accountMapper;

    @Autowired
    private TradeRecordMapper tradeRecordMapper;

    @Resource(name = "redisTemplate")
    private HashOperations<String, String, TradeRecord> hashOperations;

    @Transactional
    public TradeRecord NewTradeRecord(TradeRecord record) throws Exception {
        int ret = tradeRecordMapper.InsertRecord(record);
        if (ret == 1) {
            Long accountId = record.getAccount_id();
            String key = "TradeRecord:" + Long.toString(accountId);
            hashOperations.put(key, record.getId(), record);
        }
        return record;
    }

    public List<TradeRecord> getRecordByAccountIdFromDB(long accountId) {
        return tradeRecordMapper.selectRecordByAccountId(accountId);
    }

    public List<TradeRecord> getRecordByAccountIdFromCacheManual(long accountId) {
        String key = "TradeRecord:" + Long.toString(accountId);
        return hashOperations.values(key);
    }

    public int getRecordCntFromDB() {
        return tradeRecordMapper.getRecordCnt();
    }

    public int getRecordCntByAccountIdFromCache(long accountId) {
        String key = "TradeRecord:" + Long.toString(accountId);
        return hashOperations.size(key).intValue();
    }
}
