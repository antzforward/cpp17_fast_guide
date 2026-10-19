# UE CLI 实验台

在真实 UE 中学习 API、数据流、生命周期、调用顺序和参数选择。日常写一个实验 `.cpp`，从命令行构建、运行、查看检查和测量结果。共用宿主负责引擎初始化，普通实验无需创建新项目。

本机基线：UE **5.6.1**（`E:\EpicGames\UE_5.6`）、MSVC **14.38.33145**、Windows SDK **10.0.22621.0**。这是安装版引擎，宿主使用最小 Editor target + Commandlet，复用预编译引擎模块。运行采用 NullRHI，无需打开编辑器窗口。C++ 标准和构建定义跟随 UE，此版本使用 C++20。

## 从仓库根目录开始

在普通 CMD 中运行，不需要先手动进入 VS 开发者命令行：

```bat
cd /d G:\cpp17_fast_guide
ue.bat doctor
ue.bat list
ue.bat run probe
ue.bat run array --set Count=10000 --set Reserve=1 --set Shrink=0
ue.bat run flow --set Count=4096 --set Scale=3
ue.bat run lifetime
ue.bat run transport --set Count=512 --set BatchSize=64 --set Quantum=10
```

`ue.bat` 使用本机现有 Node.js，直接调用引擎自带 .NET/UBT 和 `UnrealEditor-Cmd.exe`，无需 Python。首次构建要生成宿主、反射代码和共享预编译头；后续切换普通实验主要编译一个文件并链接实验模块。以本机实际报告为准。

另一台机器可创建不入版本库的 `settings.local.json`：

```json
{ "engineRoot": "D:/EpicGames/UE_5.6" }
```

也可设置环境变量 `UE_LAB_ENGINE`。更换引擎版本需要重新构建并运行验证；当前只验收 UE 5.6.1。设置错误时先跑 `doctor`。

## 已提供的真实实验

| 实验 | 观察内容 | 可调参数 / 关键结果 |
| --- | --- | --- |
| `probe` | 真实 TArray、FName、当前引擎与线程 | FName 大小、实际容量，均只对应当前配置 |
| `array` | 容量轨迹、预留、无序删除、显式收缩 | `Count`、`Reserve=0/1`、`Shrink=0/1`；增长次数与分配字节 |
| `flow` | 主线程生成 → UE 工作线程处理 → 主线程消费 → 委托反馈 | `Count`、`Scale`；各阶段数量、校验和、线程 ID |
| `lifetime` | NewObject → Root → 弱绑定 → GC → 失效回调跳过 | 两次真实 GC；弱引用和回调次数 |
| `transport` | 批量编码 → 按收益压缩 → 有界解码 → 校验反馈 | `Count`、`BatchSize`、`Quantum`、`Compression=0/1`；字节与误差 |

`flow` 用阻塞等待把数据归属与完成顺序显式展示；实时游戏应再研究依赖、continuation、取消与背压。它不调度游戏帧，也不代表渲染线程或网络路径。

`transport` 使用真实 `FMemoryWriter/FMemoryReader`、`SerializeIntPacked`、`FCompression` 和 zlib。ID 与 SLG 的 `int64` 数值精确往返；位置使用量化与 ZigZag 编码。`Quantum` 是每单位的刻度数：10 对应 0.1 的步长，最大舍入误差为 0.05。压缩变大时保留原始 payload。解码验证长度、版本、条数与上限，再分配内存；包含截断和超限输入检查。

这是同一 UE 构建中的本地内存往返。`wire_bytes` 只计本实验的 envelope 和 payload，尚未包含实际网络包开销；该格式未承诺跨版本或跨端兼容。认证、加密、完整性和重放处理在后续真实连接阶段验收。

## 用测量选择参数

```bat
ue.bat sweep transport --set Count=512 --param BatchSize=16,64,256 --param Quantum=1,10
ue.bat sweep array --set Count=10000 --param Reserve=0,1 --param Shrink=0,1 --repeat 3
```

扫描会先构建一次，再对参数组合分别启动真实引擎，输出：

- `artifacts/sweep.csv`、`sweep.json`：各组合的指标与通过状态。
- 每次运行的 `.json`、`.log`：参数、引擎版本、源文件指纹、答案类型、检查、指标与退出码。
- `artifacts/last.json`：最近一次运行；`build-last.log`：最近一次构建日志。

先给约束，再找参数：例如“位置误差不超过 0.05、处理时间在预算内，再比较字节”。批量越大未必越好，实际联网还需考虑等待时间、包大小、相关性、更新频率和重传。样例数据分布与规模也必须变化。

`wallMs` 包含引擎启动和退出；`experiment_ms` 是实验内部时间。`transport.codec_ms` 包含该进程首次压缩器初始化和编解码/校验成本，不能直接当生产吞吐量。参数比较要多次运行，性能验收不使用一个固定机器耗时阈值。重复 `--set` 名称，或让 `--set` 与 `--param` 使用同一名称，会被拒绝。

## 第一课：API 选择与参数语义

[01_array_parameters](lessons/01_array_parameters/README.md) 包含三层独立验收：Reserve 与元素创建、删除顺序与收缩策略、真实容量轨迹比较。

```bat
ue.bat check 01_array_parameters --solution
ue.bat check 01_array_parameters --solution --level 3 --set Count=8192
ue.bat check 01_array_parameters --level 1
```

`array_parameters.cpp` 是你的答题文件，只提供声明与提示；`solution_array_parameters.cpp` 是独立参考答案。第三条命令在尚未实现 Level 1 时会因缺少函数定义而链接失败，这是未答题状态。只完成一层就用 `--level 1/2/3` 单独验收，其他层不参与该次编译。省略 `--level` 检查全部三层。

参考答案通过表示环境与参考实现通过，不能替代自己的答题验收。脚本仅把选择的答案复制进临时构建位置，绝不会把参考答案覆盖到答题文件。

## 自己添加实验

复制 `templates/experiment.cpp` 为 `examples/my_experiment.cpp`，实现：

```cpp
#include "LabContext.h"

int32 RunExperiment(FLabContext& Lab)
{
    // 生成输入、调用真实 UE API、验证预期、记录指标。
    return Lab.Failures == 0 ? 0 : 1;
}
```

然后执行 `ue.bat run examples/my_experiment.cpp`。`Lab.Int/Number` 读参数，`Require` 判断结果，`Metric` 保存数值，`Note` 解释输出。新增失败分支也必须返回非零或调用 `Require(false, ...)`。

普通实验放在 `examples`，课程放在 `lessons`，均不被 UBT 全部自动编译。脚本只把当前实验放到 `Source/UELab/Private/ActiveExperiment.cpp`。不要编辑 Active 文件；并发命令通过锁拒绝，构建失败会停止，不运行上一次的旧程序。

需要自定义 `UCLASS/USTRUCT` 时，在 `Source/UELab/Public` 与 `Private` 中添加正式 `.h/.cpp`，由 UHT 生成反射代码；单文件便利不要求跳过引擎机制。需要新模块时修改 `UELab.Build.cs` 并记录理由。进入源码调试可用 VS 启动 `UnrealEditor-Cmd.exe`，参数按报告日志中的项目和 `-run=UELab` 配置，本机已安装引擎调试符号。

## 环境验收与学习路线

```bat
ue.bat verify
```

此命令执行真实引擎探针、示例、参考答案的全部与单级验收，并检查非法参数会失败、空答题骨架会链接失败。它不修改答题文件。结果在 `artifacts/verification.json`；若答题文件已有实现，自动跳过“空骨架应失败”这一项。

2026-10-13 本机已通过 17 项流程检查与 6 组参数扫描，见 [VALIDATION](docs/VALIDATION.md)。为避开 UBT 的一秒时间戳容差，每次构建强制重编当前实验的一个对象文件；宿主、PCH 和引擎模块缓存继续复用。

继续学习见 [WORKFLOW](docs/WORKFLOW.md) 和 [ROADMAP](docs/ROADMAP.md)。路线将 API 探针接到参数扫描、项目情境、World/Timer/Tick、真实服务端与客户端、引擎数据交接，最后加入最小真实 RHI 客户端。

当前的 Commandlet 不自动创建游戏 World；NullRHI 不能验证 GPU 完成或画面。涉及哪一层结论，就增加对应运行环境和验收，不要求一次建立大型游戏项目。
