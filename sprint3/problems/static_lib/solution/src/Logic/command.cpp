#include "command.h"
#include "logger.h"


namespace programm_option {

    using namespace std::literals;

    [[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
        namespace po = boost::program_options;

        po::options_description desc{ "All options"s };
        Args args;
        desc.add_options()
            ("help,h", "produce help message")
            ("tick-period,t", po::value(&args.tick_period)->value_name("milliseconds"s), "set tick period")
            ("config-file,c", po::value(&args.config_file)->value_name("file"s), "set config file path")
            ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set static files root")
            ("randomize-spawn-points", po::value(&args.randomize_spawn_points), "spawn dogs at random positions");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.contains("help"s)) {
            std::cout << desc;
            return std::nullopt;
        }

        if (!vm.contains("config-file"s)) {
            std::string error_msg = "Config file have not been specified"s;
            BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(error_msg,
                logger::ExitCodeLog(EXIT_FAILURE));
            throw ConfigFileNotSpecifiedException();
        }

        if (!vm.contains("www-root"s)) {
            std::string error_msg = "Static content path is not specified"s;
            BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(error_msg,
                logger::ExitCodeLog(EXIT_FAILURE));
            throw StaticContentPathNotSpecifiedException();
        }

        return args;
    };

}