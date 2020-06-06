package com.muggle.use.spring;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.config.ConfigurableBeanFactory;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

@Component
@Scope(ConfigurableBeanFactory.SCOPE_PROTOTYPE)
public class CalcTask {
    private IdAlloc idAlloc;
    private ResAlloc resAlloc;

    private int taskId;

    @Autowired
    public CalcTask(IdAlloc idAlloc, ResAlloc resAlloc) {
        this.idAlloc = idAlloc;
        this.resAlloc = resAlloc;
        taskId = idAlloc.allocate();
    }

    public void run() {

        Integer cpu = null;
        Integer mem = null;
        try {
            cpu = resAlloc.allcCpu();
            mem = resAlloc.allcMem();

            System.out.println("run calc task: " +
                    "id=" + taskId +
                    ", cpu[" + cpu + "]" +
                    ", mem[" + mem + "]");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            if (cpu != null) {
                resAlloc.freeCpu(cpu);
            }
            if (mem != null) {
                resAlloc.freeMem(mem);
            }
        }
    }
}
