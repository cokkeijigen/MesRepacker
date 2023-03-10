# MesRepacker
mes脚本重打包 

## 如何使用
### 导出文本
将mes文件或者文件夹拖动到exe
### 导入文本
首先确保你的文本格式为`#0xabc11: 文本` 如下图所示  
![Image text](https://github.com/cokkeijigen/MesRepacker/blob/main/demonstrate.png)<br>
然后放有文本的文件夹里包含`.MesRepacker`文件，这个文本内容是你的源（未修改）的mes路径
将整个文件夹拖动到exe上即可
 
默认情况下，程序会将sj编码转换成utf8然后导出  
此外还支持gbk转utf8，需要exe的重命名加上`-igbk`，例如`MesRepacker-igbk.exe`  
如果不需要转换，将exe的重命名加上`-nsc`，例如`MesRepacker-nsc.exe`  

程序默认是自动选择mes版本，但是你可以手动选择  
将exe重命名加上`-s版本简称`，例如`MesRepacker-sdc4.exe`// 配置为dc4
| 游戏| 简称 | 
| :-----| :----: |
|D.S. -Dal Segno- | ds | 
|D.C.4 ～ダ・カーポ4～ | dc4|
|D.C.Ⅲ DreamDays|dc3dd|
|D.C.III WithYou|dc3wy|
|D.C.III RX-rated|dc3rx|
|D.C.II P.S.|dc2pc|
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
|Ｄ．Ｃ．Ｐ．Ｃ．(Vista)|dcpcv|
|D.C. Summer Vacation|dcsv|
|D.C.WhiteSeason|dcws|
|ことり Love Ex P|ktlep|
|D.C.〜ダ・カーポ〜　温泉編|dcos|
