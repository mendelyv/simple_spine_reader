# spine runtime 数据读取

简易的spine二进制导出文件读取，用于检查动画名称是否在配置白名单中

## 依赖
- c++17，使用了filesystem标准库
- [spine runtime]c++源码，将仓库中的spine-cpp文件夹复制到工程目录下。如果使用dll，请自行编译并修改cmake文件。
- [json]用于解析json文件，将仓库中release的json.hpp复制到include目录下。如果想自定义路径，请自行修改cmake文件。

## 编译
- windows: 使用cmake生成vs工程，并编译。

## 使用
- 读取config文件，配置文件需于可执行文件同级
    
    - ``` json
      {
        "rootPath": ".", --路径，默认为可执行文件同级
        "pause": true, --执行完成是否暂停
        "silent": false, --静默执行
        "checkLogs": true, --输出log文件
        "details": true, --输出spine细节文件
        "logHasAttachmentSlot", --只输出有数据的插槽
        "names": ["attack"] --动画名称白名单
      }
      ```

- 递归读取根路径下的文件，找到所有的spine导出结构文件

- 读取spine文件，匹配动画名称，输出日志

  

[spine runtime]: https://github.com/EsotericSoftware/spine-runtimes/tree/4.2/spine-cpp
[json]: https://github.com/nlohmann/json
