#include "../include/Exceptions.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <execinfo.h>
#include <cxxabi.h>
#include <cstring>

namespace GUI {

std::map<GUIException::ErrorCode, std::string> GUIException::errorCodeNames = {
    {ErrorCode::UNKNOWN_ERROR, "Unknown Error"},
    {ErrorCode::WINDOW_ERROR, "Window Error"},
    {ErrorCode::WIDGET_ERROR, "Widget Error"},
    {ErrorCode::FONT_ERROR, "Font Error"},
    {ErrorCode::RESOURCE_ERROR, "Resource Error"},
    {ErrorCode::INPUT_ERROR, "Input Error"},
    {ErrorCode::RENDER_ERROR, "Render Error"},
    {ErrorCode::NETWORK_ERROR, "Network Error"},
    {ErrorCode::CONFIGURATION_ERROR, "Configuration Error"},
    {ErrorCode::VALIDATION_ERROR, "Validation Error"}
};

std::map<GUIException::Severity, std::string> GUIException::severityNames = {
    {Severity::DEBUG, "DEBUG"},
    {Severity::INFO, "INFO"},
    {Severity::WARNING, "WARNING"},
    {Severity::ERROR, "ERROR"},
    {Severity::CRITICAL, "CRITICAL"}
};

std::vector<GUIException::ErrorHandler> GUIException::globalHandlers;
bool GUIException::enableStackTrace = true;

GUIException::GUIException(const std::string& msg,
                         ErrorCode errCode,
                         Severity sev,
                         const ErrorContext& ctx)
    : message(msg), code(errCode), severity(sev), context(ctx),
      timestamp(std::chrono::system_clock::now()) {

    if (enableStackTrace) {
        captureStackTrace();
    }
    notifyGlobalHandlers();
}

GUIException::GUIException(const GUIException& other)
    : message(other.message), code(other.code), severity(other.severity),
      context(other.context), timestamp(other.timestamp),
      stackTrace(other.stackTrace), customData(other.customData) {
}

GUIException& GUIException::operator=(const GUIException& other) {
    if (this != &other) {
        message = other.message;
        code = other.code;
        severity = other.severity;
        context = other.context;
        timestamp = other.timestamp;
        stackTrace = other.stackTrace;
        customData = other.customData;
    }
    return *this;
}

const char* GUIException::what() const noexcept {
    return message.c_str();
}

void GUIException::setCustomData(const std::string& key, const std::string& value) {
    customData[key] = value;
}

std::string GUIException::getCustomData(const std::string& key) const {
    auto it = customData.find(key);
    return (it != customData.end()) ? it->second : "";
}

void GUIException::addMetadata(const std::string& key, const std::string& value) {
    context.metadata[key] = value;
}

std::string GUIException::getFormattedMessage() const {
    std::ostringstream oss;

    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto tm = *std::localtime(&time_t);
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ";

    oss << "[" << getSeverityString() << "] ";
    oss << "[" << getErrorCodeString() << "] ";

    oss << message;

    if (!context.function.empty() || !context.file.empty()) {
        oss << " (at " << context.function;
        if (!context.file.empty()) {
            oss << " in " << context.file;
            if (context.line > 0) {
                oss << ":" << context.line;
            }
        }
        oss << ")";
    }

    return oss.str();
}

std::string GUIException::getErrorCodeString() const {
    return getErrorCodeName(code);
}

std::string GUIException::getSeverityString() const {
    return getSeverityName(severity);
}

void GUIException::addGlobalHandler(ErrorHandler handler) {
    globalHandlers.push_back(handler);
}

void GUIException::removeGlobalHandler(std::size_t index) {
    if (index < globalHandlers.size()) {
        globalHandlers.erase(globalHandlers.begin() + index);
    }
}

std::string GUIException::getErrorCodeName(ErrorCode code) {
    auto it = errorCodeNames.find(code);
    return (it != errorCodeNames.end()) ? it->second : "Unknown Error Code";
}

std::string GUIException::getSeverityName(Severity sev) {
    auto it = severityNames.find(sev);
    return (it != severityNames.end()) ? it->second : "UNKNOWN";
}

void GUIException::logException(const GUIException& ex) {
    std::cerr << ex.getFormattedMessage() << std::endl;

    if (!ex.getStackTrace().empty()) {
        std::cerr << "Stack trace:" << std::endl;
        for (std::size_t i = 0; i < ex.getStackTrace().size(); ++i) {
            std::cerr << "  " << i << ": " << ex.getStackTrace()[i] << std::endl;
        }
    }

    if (!ex.getAllCustomData().empty()) {
        std::cerr << "Custom data:" << std::endl;
        for (const auto& pair : ex.getAllCustomData()) {
            std::cerr << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
}

void GUIException::captureStackTrace() {
    const int maxFrames = 64;
    void* callstack[maxFrames];
    int frames = backtrace(callstack, maxFrames);
    char** symbols = backtrace_symbols(callstack, frames);

    if (symbols != nullptr) {
        for (int i = 1; i < frames; ++i) {
            std::string symbol = symbols[i];

            std::size_t start = symbol.find('(');
            std::size_t end = symbol.find('+');
            if (start != std::string::npos && end != std::string::npos && start < end) {
                std::string mangled = symbol.substr(start + 1, end - start - 1);
                int status;
                char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
                if (demangled != nullptr) {
                    symbol = symbol.substr(0, start + 1) + demangled + symbol.substr(end);
                    free(demangled);
                }
            }

            stackTrace.push_back(symbol);
        }
        free(symbols);
    }
}

void GUIException::notifyGlobalHandlers() const {
    for (const auto& handler : globalHandlers) {
        try {
            handler(*this);
        } catch (...) {
            std::cerr << "Warning: Exception handler threw an exception" << std::endl;
        }
    }
}

WindowException::WindowException(const std::string& msg,
                               WindowErrorType type,
                               const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::WINDOW_ERROR, Severity::ERROR, ctx),
      windowErrorType(type) {
}

std::string WindowException::getWindowErrorTypeString() const {
    switch (windowErrorType) {
        case WindowErrorType::CREATION_FAILED: return "Creation Failed";
        case WindowErrorType::ALREADY_OPEN: return "Already Open";
        case WindowErrorType::ALREADY_CLOSED: return "Already Closed";
        case WindowErrorType::INVALID_SIZE: return "Invalid Size";
        case WindowErrorType::INVALID_TITLE: return "Invalid Title";
        case WindowErrorType::RENDER_FAILED: return "Render Failed";
        case WindowErrorType::EVENT_HANDLING_FAILED: return "Event Handling Failed";
        default: return "Unknown Window Error";
    }
}

WidgetException::WidgetException(const std::string& msg,
                               WidgetErrorType type,
                               const std::string& id,
                               const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::WIDGET_ERROR, Severity::ERROR, ctx),
      widgetErrorType(type), widgetId(id) {
}

std::string WidgetException::getWidgetErrorTypeString() const {
    switch (widgetErrorType) {
        case WidgetErrorType::INVALID_POSITION: return "Invalid Position";
        case WidgetErrorType::INVALID_SIZE: return "Invalid Size";
        case WidgetErrorType::ID_CONFLICT: return "ID Conflict";
        case WidgetErrorType::PARENT_NOT_FOUND: return "Parent Not Found";
        case WidgetErrorType::CHILD_ADDITION_FAILED: return "Child Addition Failed";
        case WidgetErrorType::RENDER_FAILED: return "Render Failed";
        case WidgetErrorType::EVENT_HANDLING_FAILED: return "Event Handling Failed";
        case WidgetErrorType::INVALID_STATE: return "Invalid State";
        default: return "Unknown Widget Error";
    }
}

FontException::FontException(const std::string& msg,
                           FontErrorType type,
                           const std::string& path,
                           const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::FONT_ERROR, Severity::ERROR, ctx),
      fontErrorType(type), fontPath(path) {
}

std::string FontException::getFontErrorTypeString() const {
    switch (fontErrorType) {
        case FontErrorType::LOAD_FAILED: return "Load Failed";
        case FontErrorType::NOT_FOUND: return "Not Found";
        case FontErrorType::INVALID_SIZE: return "Invalid Size";
        case FontErrorType::RENDER_FAILED: return "Render Failed";
        case FontErrorType::CHARACTER_NOT_FOUND: return "Character Not Found";
        case FontErrorType::MEMORY_ERROR: return "Memory Error";
        default: return "Unknown Font Error";
    }
}

ResourceException::ResourceException(const std::string& msg,
                                   ResourceErrorType type,
                                   const std::string& path,
                                   const std::string& resourceType,
                                   const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::RESOURCE_ERROR, Severity::ERROR, ctx),
      resourceErrorType(type), resourcePath(path), resourceType(resourceType) {
}

std::string ResourceException::getResourceErrorTypeString() const {
    switch (resourceErrorType) {
        case ResourceErrorType::LOAD_FAILED: return "Load Failed";
        case ResourceErrorType::NOT_FOUND: return "Not Found";
        case ResourceErrorType::INVALID_FORMAT: return "Invalid Format";
        case ResourceErrorType::MEMORY_ERROR: return "Memory Error";
        case ResourceErrorType::PERMISSION_DENIED: return "Permission Denied";
        case ResourceErrorType::CORRUPTED_DATA: return "Corrupted Data";
        default: return "Unknown Resource Error";
    }
}

InputException::InputException(const std::string& msg,
                             InputErrorType type,
                             const std::string& value,
                             const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::INPUT_ERROR, Severity::WARNING, ctx),
      inputErrorType(type), inputValue(value) {
}

std::string InputException::getInputErrorTypeString() const {
    switch (inputErrorType) {
        case InputErrorType::INVALID_INPUT: return "Invalid Input";
        case InputErrorType::VALIDATION_FAILED: return "Validation Failed";
        case InputErrorType::PARSING_ERROR: return "Parsing Error";
        case InputErrorType::CONVERSION_FAILED: return "Conversion Failed";
        case InputErrorType::LENGTH_EXCEEDED: return "Length Exceeded";
        case InputErrorType::FORMAT_ERROR: return "Format Error";
        default: return "Unknown Input Error";
    }
}

RenderException::RenderException(const std::string& msg,
                               RenderErrorType type,
                               const sf::Vector2f& pos,
                               const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::RENDER_ERROR, Severity::ERROR, ctx),
      renderErrorType(type), renderPosition(pos) {
}

std::string RenderException::getRenderErrorTypeString() const {
    switch (renderErrorType) {
        case RenderErrorType::DRAW_FAILED: return "Draw Failed";
        case RenderErrorType::TEXTURE_ERROR: return "Texture Error";
        case RenderErrorType::SHADER_ERROR: return "Shader Error";
        case RenderErrorType::BUFFER_ERROR: return "Buffer Error";
        case RenderErrorType::CONTEXT_ERROR: return "Context Error";
        case RenderErrorType::PERFORMANCE_ERROR: return "Performance Error";
        default: return "Unknown Render Error";
    }
}

ConfigurationException::ConfigurationException(const std::string& msg,
                                             ConfigErrorType type,
                                             const std::string& key,
                                             const std::string& value,
                                             const ErrorContext& ctx)
    : GUIException(msg, ErrorCode::CONFIGURATION_ERROR, Severity::ERROR, ctx),
      configErrorType(type), configKey(key), configValue(value) {
}

std::string ConfigurationException::getConfigErrorTypeString() const {
    switch (configErrorType) {
        case ConfigErrorType::INVALID_CONFIG: return "Invalid Config";
        case ConfigErrorType::MISSING_KEY: return "Missing Key";
        case ConfigErrorType::INVALID_VALUE: return "Invalid Value";
        case ConfigErrorType::PARSE_ERROR: return "Parse Error";
        case ConfigErrorType::FILE_NOT_FOUND: return "File Not Found";
        case ConfigErrorType::PERMISSION_DENIED: return "Permission Denied";
        default: return "Unknown Config Error";
    }
}

} // namespace GUI
