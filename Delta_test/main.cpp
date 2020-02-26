#include "qcopyworker.h"

#include "toml11/toml.hpp"

#include <QCoreApplication>
#include <QThreadPool>
#include <list>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

/*    std::ofstream test_file("first.dat", std::ios_base::binary);
    char buff[1048576];

    for (int var = 0; var < 1000; ++var) {
        test_file.write(buff, 1048576);
    }

    test_file.close();*/


        QThreadPool workersPool;
        std::ifstream ifs("Files_to_copy.toml", std::ios_base::binary);

        if (not ifs.good())
            std::cout<< "Nope!";

        const auto data = toml::parse(ifs);
        //const auto tdata = toml::find(data, "files");
        const int max_threads = toml::find_or<int>(data, "max_threads", 2);
        const auto dist = toml::find_or<std::string>(data, "dist", "");
        auto files = toml::get<std::list<std::string>>(toml::find(data, "files"));

        workersPool.setMaxThreadCount(max_threads);

        QCopyWorker* temp;
        while(!files.empty())
        {
            temp = new QCopyWorker(files.front(), dist);
            workersPool.start(temp, QThread::Priority::NormalPriority);
            files.pop_front();
        }

        ifs.close();

    return a.exec();
}
