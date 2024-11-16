if (entry.path().extension() == ".rst")
{

    std::ifstream inputFile(entry.path());
    std::ofstream outputFile(target_path);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        std::cerr << "open file error" << std::endl;
        inputFile.close();
        outputFile.close();
        continue;
    }

    std::string line      = "";
    std::string line_last = "";
    std::string chapter;
    std::regex title_pattern("^[=\\-`:\\'\\\"~\\^\\_\\*\\+\\#\\<\\>]+$");

    while (std::getline(inputFile, line))
    {
        // 检测章节标题
        if (std::regex_search(line, title_pattern) && line.size() >= line_last.size())
        {
            if (!chapter.empty())
            {

                outputFile << Separator_cut << "\n";
                outputFile << chapter;
                chapter.clear();
            }
        }

        chapter += line_last + "\n";
        line_last = line;
    }

    chapter += line_last + "\n";

    // 添加最后一个章节的内容（如果有的话）
    if (!chapter.empty())
    {
        outputFile << Separator_cut << "\n";
        outputFile << chapter;
    }

    inputFile.close();
    outputFile.close();
}