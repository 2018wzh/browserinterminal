#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

int main()
{
    std::string cases_dir = "..\\cases";
    std::string exe_path = "./browserinterminal.exe";
    int passed = 0, failed = 0;

    for (const auto &entry : fs::directory_iterator(cases_dir))
    {
        if (entry.path().extension() == ".in")
        {
            std::string in_file = entry.path().string();
            fs::path out_path = entry.path();
            std::string out_file = out_path.replace_extension(".out").string();
            std::string tmp_output = "tmp_output.txt";

            // 构建命令行指令
            std::string command = exe_path + " " + in_file + " " + tmp_output;

            // 执行命令
            int result = system(command.c_str());
            if (result != 0)
            {
                std::cout << entry.path().filename() << ": EXECUTION FAILED\n";
                failed++;
                continue;
            }

            // 读取程序输出
            std::ifstream program_output(tmp_output);
            if (!program_output.is_open())
            {
                std::cout << entry.path().filename() << ": FAILED TO OPEN OUTPUT FILE\n";
                failed++;
                continue;
            }
            std::string program_result((std::istreambuf_iterator<char>(program_output)),
                                       std::istreambuf_iterator<char>());
            program_output.close();

            // 读取预期输出
            std::ifstream expected_output(out_file);
            if (!expected_output.is_open())
            {
                std::cout << entry.path().filename() << ": FAILED TO OPEN EXPECTED OUTPUT FILE\n";
                failed++;
                continue;
            }
            std::string expected_result((std::istreambuf_iterator<char>(expected_output)),
                                        std::istreambuf_iterator<char>());
            expected_output.close();

            // 比较输出
            if (program_result == expected_result)
            {
                std::cout << entry.path().filename() << ": PASSED\n";
                passed++;
            }
            else
            {
                std::cout << entry.path().filename() << ": FAILED\n";
                failed++;
            }

            // 删除临时输出文件
            fs::remove(tmp_output);
        }
    }

    std::cout << "Total: " << (passed + failed) << ", Passed: " << passed << ", Failed: " << failed << "\n";
    return 0;
}
