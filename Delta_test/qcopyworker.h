#pragma once

#ifndef QCOPYWORKER_H
#define QCOPYWORKER_H

#include <QRunnable>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

const int BUFFER_SIZE = 1024*128;

class QCopyWorker : public QRunnable
{
public:
    explicit QCopyWorker(std::string path, std::string dist_path);
    ~QCopyWorker();

protected:
    void run();

private:
    std::string path_;
    std::string dist_path_;
    std::string file_name_;
    std::ifstream in_file_;
    std::ofstream out_file_;
    std::streamsize bs_;
    char buffer_[BUFFER_SIZE];
    long long file_size_ {0};
    std::chrono::time_point<std::chrono::high_resolution_clock> t_st_point_;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_end_point_;

};

#endif // QCOPYWORKER_H
