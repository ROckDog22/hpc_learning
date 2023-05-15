# 认证考题 : mini-spider
## 【1、内容描述】
在调研过程中，经常需要对一些网站进行定向抓取。由于python包含各种强大的库，使用python做定向抓取比较简单。请使用python开发一个迷你定向抓取器mini_spider.py，实现对种子链接的广度优先抓取，并把URL符合特定pattern的网页保存到磁盘上。
## 【2、 程序要求】
要求使用Python3，鼓励使代码支持setuptools。
程序运行:
python mini_spider.py -c spider.conf
配置文件spider.conf:
[spider]
seed_file: ./urls # 种子文件路径
result_file: ./result.data # 抓取结果存储文件, 一行一个
max_depth: 6 # 最大抓取深度(种子为0级)
crawl_interval: 1 # 抓取间隔. 单位: 秒
crawl_timeout: 2 # 抓取超时. 单位: 秒
target_url_pattern: .*.(gif|png|jpg|bmp)$ # 需要存储的目标网页URL pattern(正则表达式)
thread_count: 8 # 抓取线程数
种子文件urls:
http://cup.baidu.com/spider/
## 【3. 考察点】
a. 支持命令行参数具体包含: -h(帮助)、-v(版本)、-c(配置文件)
b. 抓取策略
- 广度优先的网页抓取策略
- 多线程抓取要求
- 获取图片文件地址并存储到文件(gif|png|jpg|bmp为扩展格式的 url, 绝对路径存储到result.data文件中, 一行一个)，为防止出现 bug 导致内部服务器流量异常, 不再要求大家必须获取并存储图片文件，将路径存储到result.data即可
c. 健壮性要求
- 单个网页抓取或解析失败，不能导致程序退出。在日志中记录错误原因并继续
- 当程序完成所有抓取任务后，优雅退出
- 从HTML提取链接时支持处理相对路径及绝对路径。
- 需要能够处理不同字符编码的网页(例如utf-8或gb18030)。
d. 优雅实现
- 代码严格遵守百度python编码规范
- 代码的可读性和可维护性好。注意模块、类、函数的设计和划分
- 完成相应的单元测试和使用demo。你的demo必须可运行，单元测试有效而且通过