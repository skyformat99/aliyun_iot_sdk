# aliyun_iot_sdk
fork from aliyun iot c sdk 2.3.0， change makefile mode

1. 原先的阿里云 iot版本，采用Cmake + 内核配置模式来编译，对交叉编译的应用很不友好，
因此改造make方式。 编译时只需更改 CC、AR、STRIP 的环境变量即可切换对不同平台的编译。
