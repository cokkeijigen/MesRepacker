# MesRepacker
### 重新打包MesScript中的文本，本工具仅供技术学习交流使用，禁止一切不法使用！  
***
### 导出文本
将mes文件或者文件夹拖动到exe
### 导入文本
首先确保你的文本目录下存在`.MesRepacker`这个文件,然后将整个文本目录到exe上即可<br>
![Image text](https://github.com/cokkeijigen/MesRepacker/blob/main/picture.png)<br>

`.MesRepacker`参数详细<br>
```
#InputPath ; 这个是源（未修改）mes文件的目录
D:\xxxxx\xxxx\xxxx\MES

#UseCodePage ; 导入需要转换的编码(CodePage), 使用-nsc之后将不支持
936 ; 默认： utf-8(65001) -> gbk(936)

#Before-Replaces ; 格式化前替换文本
[]:[] ; 格式： [原来文本]:[替换文本] 支持替换多个，使用换行区分

#After-Replaces ; 格式化后替换文本
[]:[] ; 格式同上

```
***
默认情况下，程序会将sj编码转换成utf8然后导出  
此外还支持gbk转utf8，需要exe的重命名加上`-igbk`，例如`MesRepacker-igbk.exe`  
如果不需要转换，将exe的重命名加上`-nsc`，例如`MesRepacker-nsc.exe`  

程序默认是自动选择mes版本，但是你可以手动选择  
将exe重命名加上`-s版本简称`，例如`MesRepacker-sdc4.exe`// 配置为dc4
| 游戏| 简称 | 
| :-----| :----: |
|Eternal Fantasy | ef | 
|D.S. -Dal Segno- | ds | 
|D.C.4 ～ダ・カーポ4～ | dc4|
|D.C.III DreamDays|dc3dd|
|D.C.III WithYou|dc3wy|
|D.C.III RX-rated|dc3rx|
|D.C.II P.C.|dc2pc|
|D.C.II To You|dc2ty|
|D.C.II Spring Celebration|dc2sc|
|D.C.II C.C. 音姫先生のどきどき特別授業|dc2ccotm|
|D.C.II C.C. 月島小恋のらぶらぶバスルーム|dc2cckko|
|D.C.II 〜featuring　Yun2〜|dc2fy|
|D.C.II Dearest Marriage|dc2dm|
|D.C.II 春風のアルティメットバトル！|dc2bs・dcbs|
|D.C.II Fall in Love|dc2fl|
|D.C.A.S. 〜ダ・カーポ〜アフターシーズンズ|dcas|
|D.C. Dream X’mas|dcdx|
|D.C.〜ダ・カーポ〜　MEMORIES DISC|dcmems|
|Ｄ．Ｃ．Ｐ．Ｃ．(Vista)|dcpc|
|D.C. Summer Vacation|dcsv|
|D.C.WhiteSeason|dcws|
|ことり Love Ex P|ktlep|
|D.C.〜ダ・カーポ〜　温泉編|dcos|
