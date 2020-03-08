#include "threadpool.h"

#include "toml11/toml.hpp"

#include <QCoreApplication>
#include <mutex>
#include <time.h>


std::mutex rw_mutex;
std::mutex cout_mutex;
std::mutex error_mutex;


void CopyFile(std::string inPath, std::string distPath,
              std::list<std::pair<std::string, std::string>>& exceptionList,
              unsigned int maxRate = 0)
{
    const unsigned int BUFFER_SIZE = 1024*64;
    const unsigned int rate = 1000 * 1000 * maxRate;
    char buffer[BUFFER_SIZE];
    long long file_size {0};

    std::chrono::time_point<std::chrono::high_resolution_clock> t_st_point_;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_end_point_;

    std::ifstream in_file;
    std::ofstream out_file;
    std::streamsize bs;

    std::string file_name = inPath.substr(inPath.find_last_of("\\")+1);
    distPath.append(file_name);

    try
    {
//        throw std::runtime_error("ERROR !");
        in_file.open(inPath, std::ios_base::binary);
        out_file.open(distPath, std::ios_base::binary);


        t_st_point_ = std::chrono::high_resolution_clock::now();

        if (!maxRate)
            while (!in_file.eof())
            {
                in_file.read(buffer, BUFFER_SIZE);
                bs = in_file.gcount();
                file_size += bs;
                out_file.write(buffer, bs);
            }
        else
        {
            std::chrono::time_point<std::chrono::high_resolution_clock> t2_st_point_;
            std::chrono::time_point<std::chrono::high_resolution_clock> t2_end_point_;
            long long processed_size {0};

            t2_st_point_ = std::chrono::high_resolution_clock::now();

            std::unique_lock<std::mutex> lock(rw_mutex);

            while(!in_file.eof())
            {
                in_file.read(buffer, BUFFER_SIZE);
                bs = in_file.gcount();
                processed_size += bs;
                out_file.write(buffer, bs);

                if (processed_size >= rate)
                {
                    lock.unlock();
                    file_size += processed_size;
                    processed_size = 0;
                    t2_end_point_ = std::chrono::high_resolution_clock::now();
                    long long aprox_rw_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2_end_point_ - t2_st_point_).count();
                    if (aprox_rw_duration < 1000000)
                        std::this_thread::sleep_for(std::chrono::microseconds(1000000 - aprox_rw_duration));
                    lock.lock();

                    t2_st_point_ = std::chrono::high_resolution_clock::now();
                }
            }

            lock.unlock();
    //        t2_st_point_ = std::chrono::high_resolution_clock::now();
    //            in_file.read(buffer, BUFFER_SIZE);
    //            bs = in_file.gcount();
    //            file_size += bs;
    //            out_file.write(buffer, bs);
    //        t2_end_point_ = std::chrono::high_resolution_clock::now();
    //        long long aprox_rw_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2_end_point_ - t2_st_point_).count();

    //        unsigned int chunks_count = (maxRate * 1024 * 1024) / BUFFER_SIZE;
    //        unsigned int delta = static_cast<unsigned int>((1000000 - chunks_count * aprox_rw_duration)/chunks_count);

    //        while(!in_file.eof())
    //        {
    //            in_file.read(buffer, BUFFER_SIZE);
    //            bs = in_file.gcount();
    //            file_size += bs;
    //            out_file.write(buffer, bs);
    //            std::this_thread::sleep_for(std::chrono::microseconds(delta));
    //        }

    //        std::cout<< "\nDelta: " << delta <<
    //                    "\nChunks count: " << chunks_count << std::endl;
        }

        t_end_point_ = std::chrono::high_resolution_clock::now();
    }

    catch (std::exception& e)
    {
        in_file.close();
        out_file.close();

        std::lock_guard<std::mutex> lock(error_mutex);
        exceptionList.push_back(std::pair<std::string, std::string>(file_name, e.what()));
        return;
    }

    in_file.close();
    out_file.close();

    long long elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(t_end_point_ - t_st_point_).count();

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout<< file_name <<" is copied, file size is " <<(file_size / 1024) <<
                    " KB. Elapsed time: " << elapsed_time <<  " seconds" << std::endl <<
                    "Speed: " << (file_size / (1024 * elapsed_time))<<
                    " KB\\sec" << std::endl;
    }
    return;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    std::ofstream test_file("fourth.dat", std::ios_base::binary);
//    char buff[1048576];

//    for (int var = 0; var < 1000; ++var) {
//        test_file.write(buff, 1048576);
//    }

//    test_file.close();

    ThreadPool tPool;
    std::list<std::pair<std::string, std::string>> ex_list;
    std::ifstream ifs("Files_to_copy.toml", std::ios_base::binary);

    if (not ifs.good())
    {
        std::cout<< "Toml file is not found!\n";
        a.exit();
    }

    const auto data = toml::parse(ifs);
    ifs.close();
    //const auto tdata = toml::find(data, "files");
    const unsigned char max_threads = toml::find_or<unsigned char>(data, "max_threads", 2);
    const auto max_rate = toml::find_or<int>(data, "max_rate", 0);
    const auto dist = toml::find_or<std::string>(data, "dist", "");
    auto files = toml::get<std::list<std::string>>(toml::find(data, "files"));

    tPool.setMaxThreadsCount(max_threads);

    while(!files.empty())
    {
        tPool.add(CopyFile, files.front(), dist, std::ref(ex_list), max_rate);
        files.pop_front();
    }

    tPool.Execute();

    while(!ex_list.empty())
    {
        auto ex = ex_list.front();
        ex_list.pop_front();

        std::cout<< "\nError: " << ex.second << " with the file: " << ex.first;
    }

    return a.exec();
}
