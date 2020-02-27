#include "threadpool.h"

#include "toml11/toml.hpp"

#include <QCoreApplication>
#include <list>

const int BUFFER_SIZE = 1024*128;

void foo(std::string s)
{
    std::cout<<"Hello " << s << std::endl;
}

void CopyFile(std::string inPath, std::string distPath, unsigned int maxRate = 0)
{
    char buffer[BUFFER_SIZE];
    long long file_size {0};
    std::streamsize bs;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_st_point_;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_end_point_;

    std::string file_name = inPath.substr(inPath.find_last_of("\\")+1);
    distPath.append(file_name);
    std::ifstream in_file (inPath, std::ios_base::binary);
    std::ofstream out_file (distPath, std::ios_base::binary);


    t_st_point_ = std::chrono::high_resolution_clock::now();

        while (!in_file.eof())
        {
            in_file.read(buffer, BUFFER_SIZE);
            bs = in_file.gcount();
            file_size += bs;
            out_file.write(buffer, bs);

        }

    t_end_point_ = std::chrono::high_resolution_clock::now();


    in_file.close();
    out_file.close();
    long long elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(t_end_point_ - t_st_point_).count();
    std::cout<< file_name <<" is copied, file size is " << (file_size / 1024) << " KB. Elapsed time: " << elapsed_time <<  " seconds" << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

/*    std::ofstream test_file("first.dat", std::ios_base::binary);
    char buff[1048576];

    for (int var = 0; var < 1000; ++var) {
        test_file.write(buff, 1048576);
    }

    test_file.close();*/

    ThreadPool tPool;
    std::ifstream ifs("Files_to_copy.toml", std::ios_base::binary);

    if (not ifs.good())
        std::cout<< "Nope!";

    const auto data = toml::parse(ifs);
    //const auto tdata = toml::find(data, "files");
    const unsigned char max_threads = toml::find_or<unsigned char>(data, "max_threads", 2);
    const auto dist = toml::find_or<std::string>(data, "dist", "");
    auto files = toml::get<std::list<std::string>>(toml::find(data, "files"));

    tPool.setMaxThreadsCount(max_threads);
    tPool.add(foo, "world");
    tPool.add(foo, "Tom");
    tPool.add(foo, "Mom");
    tPool.add(foo, "Jane");
    tPool.add(foo, "Life");
    tPool.tryExecute();

    ifs.close();

    return a.exec();
}
