#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    // 初始化MPI环境
    MPI_Init(&argc, &argv);

    // 获取当前进程的排名
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // 获取总进程数
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // 打印带有排名和大小的hello world消息
    std::cout << "Hello world from rank " << world_rank << " out of " << world_size << " processors" << std::endl;

    // 清理MPI环境
    MPI_Finalize();

    return 0;
}
//运行指令 mpiexec -n N 05MpiExample.exe #N表示启动N的进程
//编译指令可以使用cl来编译链接，cl /EHsc 05MpiExample.cpp msmpi.lib