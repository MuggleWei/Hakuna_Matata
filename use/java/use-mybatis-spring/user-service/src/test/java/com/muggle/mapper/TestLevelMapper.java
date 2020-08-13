package com.muggle.mapper;

import com.muggle.entity.Level;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.util.List;

@ExtendWith(SpringExtension.class)
@ContextConfiguration("/test.xml") // jdbc:initialize-database will init db and tables
public class TestLevelMapper {
    private static final Logger logger = LoggerFactory.getLogger(TestLevelMapper.class);

    @Autowired
    private LevelMapper levelMapper;

    @Test
    public void testLevelMapper() {
        Level level = new Level();
        level.setId(10001L);
        level.setLevel(5);
        int addCnt = levelMapper.addLevel(level);
        Assertions.assertEquals(addCnt, 1);

        Level qryLevel = new Level();
        List<Level> levels = levelMapper.queryLevel(10001L);
        Assertions.assertEquals(levels.size(), 1);
        Assertions.assertEquals(levels.get(0).getLevel(), 5);
    }

    @Test
    public void testLevelAddDuplicateId() {
        Level level = new Level();
        level.setId(10002L);
        level.setLevel(5);
        int addCnt = levelMapper.addLevel(level);
        Assertions.assertEquals(addCnt, 1);

        String exceptionStr = null;
        try {
            addCnt = levelMapper.addLevel(level);
        } catch (Exception e) {
            exceptionStr = e.getMessage();
        }
        Assertions.assertNotNull(exceptionStr);
    }
}
