# Copilot Instructions for AI Coding Agents

## 项目架构概览
- 本项目为简易 make 工具实现，主要在 `testfile/` 和 `minimake/` 目录下开发。
- `testfile/main.c` 是核心入口，负责解析 makefile、处理依赖、执行命令。
- 依赖和命令以结构体数组 `data_t` 管理，支持递归依赖和命令执行。
- `graph.c`/`graph.h` 提供图结构相关操作（如依赖关系建模），但目前未深度集成。
- `readfile.c`/`readfile.h` 预留文件读取相关功能。

## 关键开发流程
- **编译**：推荐使用 VS Code 任务 `C/C++: gcc-9 生成活动文件`，或手动运行：
  ```bash
  gcc-9 -g <file.c> -o <output>
  ```
- **调试**：可用 VS Code 的 C/C++ 调试配置，或直接运行生成的可执行文件。
- **测试**：暂无自动化测试脚本，建议手动运行主程序并观察输出。
- **Makefile 处理**：主程序默认读取 `file_test_makefile`，若不存在则尝试 `makefile`。
- **输出**：结果写入 `file_test_out.mk` 或 `Minimake_cleared.mk`。

## 项目约定与模式
- 依赖和命令均以二维数组存储，最大支持 100 条规则，每条规则依赖和命令数受限于 `LINE_LENTH`。
- 命令行参数支持 `--help`、`--verbose`，并有错误提示和容错处理。
- 规则解析严格要求目标行以冒号分隔，命令行以 TAB 起头。
- 依赖项可为文件或其他目标，未找到文件时会检查是否为已定义目标。
- 结构体和函数命名采用小写加下划线风格。

## 重要文件/目录
- `testfile/main.c`：主逻辑，参数解析、规则处理、命令执行
- `testfile/graph.c`/`graph.h`：图结构操作（如 addVertex, addEdge）
- `testfile/readfile.c`/`readfile.h`：文件读取相关（待完善）
- `testfile/output/`：存放编译和运行输出
- `minimake/`：备用或实验性实现

## 典型用法示例
- 编译主程序：
  ```bash
  gcc-9 testfile/main.c -o testfile/main
  ./testfile/main --help
  ./testfile/main --verbose
  ```
- 添加新规则：在 `file_test_makefile` 中按 `目标: 依赖` 和 TAB 命令格式添加

## 其他说明
- 项目未使用第三方库，全部为标准 C 头文件。
- 代码中有大量中文注释，便于理解处理流程。
- 目前无自动化测试或 CI/CD 集成。

---
如需补充说明或发现未覆盖的开发习惯，请反馈以便迭代完善。