# i.MXRT-offline-programmer
本项目为针对NXP明星产品i.MX系列（包括i.MXRT以及i.MX6）开发的开源离线下载器，作为NXP官方在线烧录工具MFGTool的补充，理论上支持NXP所有片上带有支持SDPHost协议的BootROM的产品。该下载器可以脱机烧录加密和非加密形式的SB映像文件，方便用户工厂批量烧录。

## 版本信息

v1.0--2019.05.18

第一个版本，功能工作正常，仅支持UART boot升级；

## 支持的芯片型号

- i.MXRT1021

- i.MXRT1051, i.MXRT1052

- i.MXRT1061, i.MXRT1062, i.MXRT1064
- i.MX6UL, i.MX6ULL(理论支持，待测试)

## 开发硬件平台

本项目基于NXP FRDM-K64+MicroSD卡作为硬件平台开发。FRDM-K64可以从网上商城容易购买到，或者如果你正在使用NXP产品可以通过本地代理商申请获得。

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/FRDM-K64.png)

## 编译环境

IAR for ARM v8.32

## 使用指南

1. 代码编译通过后，将FRDM-K64的J26 USB调试口与PC机相连，然后通过板载仿真器将代码下载到FRDM-K64后，点击SW1复位按键，板载绿色LED慢闪（每隔1s闪烁一次）表明正常运行并进入Standby状态；

2. 确保SD卡插入到FRDM-K64的MicroSD卡槽，然后将板载J22即K64的USB端口连接到PC机，K64会把SD卡空间以U盘形式开放出来枚举到电脑端；

3. 在U盘根目录下新建一个flashloader文件夹和image文件夹，其中flashloader文件需要从NXP官网下载对应芯片的Flashloader tool工具包如下图，解压后找到路径Flashloader_i.MXRT1050_GA\Flashloader_RT1050_1.1\Tools\mfgtools-rel\Profiles\MXRT105X\OS Firmware（这里只以RT1050为例），把该路径下的ivt_flashloader.bin文件copy到U盘根目录flashloader文件夹下；

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/flashloader.png)

4. 用户需要使用上述flashloader工具包里面的elftosb工具把用户的s19/elf等文件最终生成的SB映像文件copy到U盘根目录image文件夹下并重命名为boot_image.sb。额外提下，除了使用elftosb工具生成sb文件之外，这里推荐下<https://blog.csdn.net/qq_36178899/article/details/84670612>网友痞子衡开发的i.MXRT Boot security tool，其最新的软件工具支持配置并生成最终的加密和非加密格式SB文件；
5. 需要把FRDM-K64的串口与目标烧写板的Boot UART连接好（注意先把RT板子上电，后把FRDM-K64 host端的UART与RT板子的串口相连，否则RT上电前UART口先有电容易导致RT启动失败），默认FRDM-K64使用UART4（PTC14/PTC15）端口作为Host端UART接口，目标烧写板以RT1052开发板为例，需要把J30和J31的跳线帽拔掉，然后把FRDM-K64 Host UART的PTC14/PTC15与RT1052板子J31/J30的2脚相连并处理好共地，最终连接效果如下图;

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/K64-UART.png)

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/RT-UART.png)

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/Connector.png)

6. 一切准备就绪之后，将RT板子的启动模式拨码开关选择Serial Dowonloader模式并复位RT板子使其生效（这一步容易忽略哈，拨码配置之后必须硬件Reset下才会触发RT重新读取Boot配置），然后每次单击FRDM-K64板子上的SW2都会触发一次烧录流程，烧录过程中绿色LED快闪，如果中间有任何错误会红色LED常亮表明错误，此时重新点击SW2即可再次触发烧录流程；
7. 除了点击SW2触发烧录之外，代码也保留了UART Shell控制台，当FRDM-K64的J26 USB调试端口插入PC的时候会虚拟一个串口出来（硬件与K64的UART0相连），打开Putty选择该COM口波特率设置为115200bps。通过该控制台可以独立触发SDP和BLHOST的命令，实际上单击SW2也可以通过该shell看到具体触发的命令，同时出现错误时也可以看到错误的原因；

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/blsh-shell.png)

8. 下面给出烧录SB文件一般需要的几条命令供参考，同时该shell也支持独立读写RT系列内部的eFuse（**另外,下面几条命令针对RT1050/RT1060有效，RT1020其sdp-write-file的地址要从0x20000000改成0x20208000，sdp-jump-address由0x20000400改成0x20208400**）。

![image](https://github.com/jicheng0622/i.MXRT-offline-programmer/blob/master/picture/command.png)

## 贡献者

该项目由jicheng0622开发并维护，欢迎共同爱好者加入contributor行列，有相关技术问题可以通过我的邮箱jicheng0622@163.com沟通交流。
