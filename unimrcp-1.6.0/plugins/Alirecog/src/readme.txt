基于阿里的语音识别sdk2.0编写的mrcp server 服务插件

已在Centos6.9-64位编译通过 和 win10 运行编译通过

目前并发2路没问题，因为阿里的免费服务只能并发2路

如果你想体验 请参考以下步骤编译

Linux：
如果你没有安装unimrcpserver 请先安装它
先安装unimrcp-deps-1.6.0依赖
cd unimrcp-deps-1.6.0
./build-dep-libs.sh

先安装1.6版本的unimrcpserver服务，解压或者先解压再上传
./bootstrap
./configure
make && make install

安装默认的路径
/usr/local/unimrcp
运行一下，是否能成功启动，启动成功后退出，编译插件

项目下载后，把 unimrcp-1.6.0\plugins 下的 Alirecog 文件夹
上传到你的unimrcpsever编译安装包里面，就是解压unimrcpserver1.6时的安装包的plugins文件夹里面

g++ -v 查看g++的版本
gcc version 4.8.2 20121015 (Red Hat 4.8.2-5) (GCC) 
如果低于4.8.2的g++版本请先更新升级，否则将无法编译通过

把unimrcp-deps-1.6.0/libs 里面的
apr 及 apr-util 依赖库建立一个软连接到 unimrcp-master/libs

注意：最新版本必须更新以下两个服务才能使用
需要更新freeswitch的unimrcp源码
需要更新unimrcpserver的源码
更新了实时语音识别，关于实时语音识别的使用请去我的csdn了解


进入插件包，准备编译
cd plugins/Alirecog
进入 plugins/Alirecog/aliSdk2.0/NlsSdkCpp2.0/lib
rm -rf 删除掉linux 的文件夹（方便，想麻烦不删也可以）
然后再解压linux ，解压完成后 进入src
plugins/Alirecog/src

make clean
make
即可编译出一个 Alireocg.so 的动态库
放到 /usr/local/unimrcp/plugin
把插件使用的unimrcpserver.xml 放到/usr/local/unimrcp/conf 里面

然后启动，测试

Window 很麻烦，改天再写