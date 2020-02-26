#include "qcopyworker.h"
#include <QtTest/QTest>


QCopyWorker::QCopyWorker(std::string path, std::string dist_path) : path_(path), dist_path_(dist_path)
{
    file_name_ = path_.substr(path_.find_last_of("\\")+1);
    std::cout<< file_name_<< " is in process.." << std::endl;
}

QCopyWorker::~QCopyWorker()
{
    in_file_.close();
    out_file_.close();
    long long elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(t_end_point_ - t_st_point_).count();
    std::cout<< file_name_ <<" is copied, file size is " << (file_size_ / 1024) << " KB. Elapsed time: " << elapsed_time <<  " seconds" << std::endl;
}

void QCopyWorker::run()
{
    in_file_.open(path_, std::ios_base::binary);
    dist_path_.append(file_name_);
    out_file_.open(dist_path_, std::ios_base::binary);

    t_st_point_ = std::chrono::high_resolution_clock::now();

    while (!in_file_.eof())
    {
        in_file_.read(buffer_, BUFFER_SIZE);
        bs_ = in_file_.gcount();
        file_size_ += bs_;
        out_file_.write(buffer_, bs_);

    }

    t_end_point_ = std::chrono::high_resolution_clock::now();

}
