#include <Windows.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>

namespace logger = SKSE::log;

constexpr auto MAIN_MENU_NAME = "Main Menu";
constexpr auto START_SCRIPT_PATH = "Data/StartupScript.bat";
constexpr auto START_SCRIPT_ENV_VAR = "SKYRIM_STARTUP_SCRIPT";

void RunScript(const std::string& scriptPath) {
    logger::info("Running script '{}'", scriptPath);
    WinExec(scriptPath.c_str(), SW_HIDE);
}

void RunStartupScript() {
    if (std::filesystem::exists(START_SCRIPT_PATH)) {
        logger::info("Found file '{}'", START_SCRIPT_PATH);
        RunScript(START_SCRIPT_PATH);
    } else {
        auto* scriptPathFromENV = std::getenv(START_SCRIPT_ENV_VAR);
        if (scriptPathFromENV) {
            logger::info("Found environment variable file path '{}'", scriptPathFromENV);
            if (std::filesystem::exists(scriptPathFromENV)) {
                logger::info("Found file '{}'", scriptPathFromENV);
                RunScript(scriptPathFromENV);
            }
        } else {
            logger::info("No startup script provided");
            logger::info("");
            logger::info("To specify, please add a script named StartupScript.bat to your Skyrim Data folder");
            logger::info("or set the SKYRIM_STARTUP_SCRIPT environment variable to the path of a script to run.");
            logger::info("");
            logger::info(
                "Note: after  you set the SKYRIM_START_SCRIPT variable, you may need to restart your mod organizer "
                "before it will be recognized.");
            logger::info(
                "Also: Data/StartupScript.bat is checked *before* SKYRIM_START_SCRIPT and will run instead if found.");
        }
    }
}

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) {
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
        return;
    }
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::info);
}

class EventProcessor : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
    EventProcessor() = default;
    ~EventProcessor() = default;
    EventProcessor(const EventProcessor&) = delete;
    EventProcessor(EventProcessor&&) = delete;
    EventProcessor& operator=(const EventProcessor&) = delete;
    EventProcessor& operator=(EventProcessor&&) = delete;

public:
    static EventProcessor& GetSingleton() {
        static EventProcessor singleton;
        return singleton;
    }
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
                                          RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
        if (event->opening && event->menuName == MAIN_MENU_NAME) RunStartupScript();
        return RE::BSEventNotifyControl::kContinue;
    }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();
    RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(&EventProcessor::GetSingleton());
    return true;
}
