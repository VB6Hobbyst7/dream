#include "../DreamCore/Common/GLHeader.h"

#include <iostream>
#include <thread>
#include <memory>

#define MINIMUM_ARGUMENTS 3

#include "DTState.h"

using namespace DreamTool;

int
main
(int argc,char** argv)
{
    #ifdef DREAM_LOG
    string logLevel = "off";
    for (int i=0; i<argc; i++)
    {
        if (string(argv[i]) == "-l")
        {
            if (argc >= i+1)
            {
                logLevel = string(argv[i+1]);
            }
        }
    }
    cout << "Using log level " << logLevel << endl;
    spdlog::set_level(spdlog::level::from_str(logLevel));
    spdlog::set_pattern("%H:%M:%S (%l) %n %v");
    auto log = spdlog::stdout_logger_mt("Main");
    log->trace("Starting...");
    #endif
    DTState state(argc,argv);
    if(!state.init())
    {
        return 1;
    }
    state.run();

    #ifdef DREAM_LOG
    log->info("Run is done. Performing stack-based clean up");
    #endif
    return 0;
}
