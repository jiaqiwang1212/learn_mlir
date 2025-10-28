# 环境配置

1. 使用`uv`创建虚拟环境，安装`notebook`

2. 进入`/data1/wangjiaqi/workspace/llvm_essentials/externals/llvm-project/llvm/utils/TableGen/jupyter`目录，运行

    ```bash
        python -m tablegen_kernel.install
    ```

3. 设置`LLVM_TBLGEN_EXECUTABLE`环境变量

    ```bash
    export LLVM_TBLGEN_EXECUTABLE=/data1/wangjiaqi/workspace/llvm_essentials/externals/llvm-project/build/bin/llvm-tblgen
    ```

4. 在当前目录下启动`jupyter notebook`