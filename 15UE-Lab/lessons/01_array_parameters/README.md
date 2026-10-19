# 01：真实 TArray——从调用 API 到选择批量参数

本课使用本机 UE 5.6.1 的真实 TArray，在共用 Commandlet 宿主中运行。你只编辑 array_parameters.cpp；不需要开编辑器、建地图或写 main。验收代码负责造数据、调用你的实现、比较结果和打印指标。

母题是“准备一批连续编号，删除失效项，再评估批量大小与预留容量”。它对应 MMORPG 状态更新列表、SLG 数值变化批次的**本地数据准备阶段**。网络传输、压缩、加密、复制条件和服务器权威性需要后续独立实验；把 TArray 的内存直接发送出去不会自动成为合法网络协议。

## 开始与自检

从仓库根目录 G:\cpp17_fast_guide 运行：

~~~bat
rem 只做 Level 1；另外两级仍是声明也不影响本级链接
ue.bat check 01_array_parameters --level 1

rem 做完后检查全部三级
ue.bat check 01_array_parameters

rem 卡住时单独运行参考答案；不会覆盖你的答题文件
ue.bat check 01_array_parameters --solution --level 1
ue.bat check 01_array_parameters --solution

rem 改变批量大小，观察容量增长与剩余容量
ue.bat check 01_array_parameters --solution --level 3 --set Count=8192
~~~

答题文件初始只有声明和 TODO，首次检查出现“函数未定义”的链接错误是正常的：先把当前级函数声明改成定义。每一级的验收由编译条件独立选入；省略 --level 时运行全部三级。Count 只用于 Level 3 的额外批次，范围为 64..1000000；固定的边界用例仍然会运行。

## Level 1：容量与元素是两个不同状态

~~~cpp
FArrayBuildResult BuildSequentialBatch(int32 First, int32 Count);
~~~

输入满足 Count >= 0，并保证 First + Count 不溢出。对空 Result.Values 先 Reserve(Count)，马上记录 NumImmediatelyAfterReserve 与 CapacityImmediatelyAfterReserve，然后依次 Emplace(First + Offset)。返回的 Values 必须恰好包含 Count 个连续整数。

验收检查：

- Reserve 之后 Num 为 0，Max 至少为 Count。
- 填完后 Num 为 Count，序列逐项正确。
- 在预留范围内填充不会再次改变 Max。
- 覆盖空批次、单元素、跨常见边界的大小，以及负数起始编号。

提示：Reserve 准备存储，不构造 Count 个元素；SetNum 改变元素数量，不能代替这一步。验收不要求 Max 精确等于 Count，因为分配器与平台可能取整。

## Level 2：根据业务契约选择删除 API 与参数

~~~cpp
void RemoveBatchEntries(TArray<int32>& Batch, int32 Index, int32 Count, bool bKeepOrder);
~~~

输入保证删除区间合法；Count == 0 也合法。bKeepOrder 为 true 时保持剩余元素的相对顺序；为 false 时选择无需保序的删除 API。两条分支都显式传入 EAllowShrinking::No，保留原有容量供下一批复用。

提示：查看 RemoveAt 与 RemoveAtSwap 的“Index, Count, AllowShrinking”重载，不要把 bool 当成第二个参数误传给 Count。当前 UE 5.6.1 使用 EAllowShrinking 枚举可以更清楚地表达意图。

验收检查：保序分支逐项比较序列；无序分支比较剩余元素的多重集合，不锁死某个版本的具体交换顺序；两者都检查数量与原容量，覆盖头部、尾部、空区间、删光和大量删除。

业务判断：

| 数据关系 | 选择理由 |
| --- | --- |
| 下标与另一组参数一一对应，或处理先后有语义 | 需要维持顺序；还要同步维护关联数据 |
| 每项带独立实体 ID，消费端按 ID 查找 | 可考虑无序删除，减少维护顺序的工作 |
| 容器反复装载相近大小的批次 | 先禁用自动收缩，观察容量与峰值需求 |
| 批次长期变小，保留容量已成为负担 | 在明确时机评估 Shrink；不要每次删除都盲目收缩 |

功能验收能确认输出契约与容量，却不能证明你确实选了更低成本的 API。完成后还要检查自己的调用选择；不能因为用 RemoveAt 实现了无序分支也得到正确集合，就认定性能目标已经完成。

## Level 3：用容量轨迹评估批量大小

~~~cpp
FArrayGrowthResult FillBatchWithGrowthTrace(int32 Count, bool bReserve);
~~~

创建空 Values。bReserve 为 true 时先 Reserve(Count)，然后记录 NumBeforeFill 与 CapacityBeforeFill。依次 Emplace 0..Count-1，并把**每次插入之后实际读取的 Values.Max()**记到 CapacityAfterEachAdd。记录轨迹的数组自身可以 Reserve，其增长不计入被观察的 Values。

验收检查：

- 两种模式的最终数据完全相同。
- 每次容量足以容纳当前元素，填充期间不会减少。
- 预留模式在填充期间的容量增长次数为 0；非空自然增长模式至少发生一次增长。
- 轨迹的最后容量与真实 Values.Max() 一致。
- 空批次、单元素、64、257 和用户指定批次分别通过。

打印指标的单位：

| 指标后缀 | 含义 |
| --- | --- |
| GrowthDuringFill | 从 CapacityBeforeFill 开始，Max 增大的次数 |
| FinalCapacity | 最终 Max，单位是元素 |
| FinalSlack | 最终 Max - Num，单位是元素 |

这些指标用于观察存储增长与剩余容量；不能直接当成复制字节量、指针改变次数、构造调用次数或墙钟耗时。Reserve 的首次容量准备发生在填充前，计数为零不代表没有分配。

建议分别用 Count=64、256、2048、8192 运行，记下增长次数与剩余容量。回答：“知道批次上限时，何时 Reserve？估计上限过大会保留多少容量？”本课不规定一个通用最佳批次值；后续需要把包大小、延迟、压缩率和服务器预算一起纳入约束。

## 与 std::vector 经验容易混淆的地方

UE 的容器实现具有自己的重定位约定。当前源码中同类型重定位可使用字节搬移，分配器也可能在原地址完成扩容。因此不能用“移动构造函数计数减少”来直接证明 TArray 的扩容成本，也不能把 std::vector 的某条增长规律作为 TArray 的验收标准。

Reserve 之后持有的元素地址也不是永久稳定的：后续超过容量的增长、删除、插入等操作仍可能改变地址或元素对应关系。以实体 ID 表达业务身份，比把数组下标当作长期身份更容易维护；具体选择取决于数据契约。

## 文件分工与源码入口

| 文件 | 用途 |
| --- | --- |
| array_parameters.cpp | 你的答题文件：只给出接口与提示，三个函数由你实现 |
| solution_array_parameters.cpp | 独立参考实现，--solution 使用它 |
| test.cpp | 造数据与独立客观验收，不调用参考答案作为判定依据 |
| ArrayParametersContract.h | 共享返回结构，防止答题与验收的接口各自变化 |

已核对本机 UE 5.6.1 的以下实现，并于 2026-10-13 实际编译运行参考答案；全部三级及各级独立验收通过，空答题的 Level 1 链接失败符合预期：

- [Array.h：Reserve、RemoveAt、RemoveAtSwap、Emplace](/E:/EpicGames/UE_5.6/Engine/Source/Runtime/Core/Public/Containers/Array.h)
- [MemoryOps.h：RelocateConstructItems 与重定位说明](/E:/EpicGames/UE_5.6/Engine/Source/Runtime/Core/Public/Templates/MemoryOps.h)

建议先在 API 处下断点，再进入分配器观察容量变化；本课使用真实引擎类型，观察到的取整和增长细节属于当前版本、平台与分配器，不应写成永久规则。

## 学习记录

- 第一次把 Reserve 和 SetNum 混淆的原因：
- 哪个业务场景必须保序，哪个允许交换删除：
- 实测 Count 与增长次数、剩余容量：
- 在源码中看到的调用链：
- 想带入自己项目进一步测量的问题：
