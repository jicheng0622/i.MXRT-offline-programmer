#include "blsh_task.h"


void blshtask(void *pvParameters)
{
    host_blsh_cmd_init();

    while (!blsh_exit())
    {
       blsh_run();
    }
    
    vTaskSuspend(NULL);
}

