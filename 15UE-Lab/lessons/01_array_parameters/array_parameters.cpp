#include "ActiveLesson/ArrayParametersContract.h"

// Level 1：返回 First, First+1, ... 共 Count 个整数。
// TODO：先对空 Values 调用 Reserve(Count)，记录调用后的 Num/Max，再用 Emplace 填充。
// Count >= 0；First + Count 不溢出。Reserve 分配容量，但不创建元素。
// 将下列声明改成你的函数定义；不要读取 solution_ 文件再复制答案。
FArrayBuildResult BuildSequentialBatch(int32 First, int32 Count);

// Level 2：从 Batch 的 Index 开始删除 Count 个元素。
// TODO：bKeepOrder 为 true 时保序，为 false 时选择无需保序的删除 API。
// 两条路径都显式禁用 shrinking；Count == 0 应合法且不改变数组。
// 输入保证 Index/Count 是合法区间，不需要处理越界。
void RemoveBatchEntries(TArray<int32>& Batch, int32 Index, int32 Count, bool bKeepOrder);

// Level 3：记录填充真实 TArray 的容量轨迹，比较预留与自然增长。
// TODO：创建空 Values；可选 Reserve(Count)；记录 NumBeforeFill/CapacityBeforeFill。
// TODO：依次 Emplace 0..Count-1，每次记录 Values.Max() 到 CapacityAfterEachAdd。
// CapacityAfterEachAdd 本身可以 Reserve，不能把它的增长算进 Values 的观测。
// 不使用耗时、对象移动构造次数或固定增长公式作为容量的替代值。
FArrayGrowthResult FillBatchWithGrowthTrace(int32 Count, bool bReserve);
