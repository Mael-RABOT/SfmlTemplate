#pragma once

#include <exception>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
#include <functional>
#include <SFML/Graphics.hpp>

namespace GUI {

/**
 * @brief Base exception class for the GUI framework
 *
 * This class provides a foundation for all custom exceptions in the GUI system.
 * It includes features like:
 * - Error codes and categories
 * - Timestamp tracking
 * - Context information
 * - Stack trace support
 * - Custom error handlers
 */
class GUIException : public std::exception {
    public:
        enum class ErrorCode {
            UNKNOWN_ERROR = 0,
            WINDOW_ERROR = 1000,
            WIDGET_ERROR = 2000,
            FONT_ERROR = 3000,
            RESOURCE_ERROR = 4000,
            INPUT_ERROR = 5000,
            RENDER_ERROR = 6000,
            NETWORK_ERROR = 7000,
            CONFIGURATION_ERROR = 8000,
            VALIDATION_ERROR = 9000
        };

        enum class Severity {
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };

        struct ErrorContext {
            std::string function;
            std::string file;
            int line;
            std::string additionalInfo;
            std::map<std::string, std::string> metadata;

            ErrorContext(const std::string& func = "", const std::string& f = "", int l = 0)
                : function(func), file(f), line(l) {}
        };

        using ErrorHandler = std::function<void(const GUIException&)>;

    private:
        std::string message;
        ErrorCode code;
        Severity severity;
        ErrorContext context;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> stackTrace;
        std::map<std::string, std::string> customData;

        static std::map<ErrorCode, std::string> errorCodeNames;
        static std::map<Severity, std::string> severityNames;
        static std::vector<ErrorHandler> globalHandlers;
        static bool enableStackTrace;

    public:
        GUIException(const std::string& msg,
                     ErrorCode errCode = ErrorCode::UNKNOWN_ERROR,
                     Severity sev = Severity::ERROR,
                     const ErrorContext& ctx = ErrorContext());

        GUIException(const GUIException& other);
        GUIException& operator=(const GUIException& other);

        virtual ~GUIException() = default;

        const char* what() const noexcept override;

        ErrorCode getErrorCode() const { return code; }
        Severity getSeverity() const { return severity; }
        const ErrorContext& getContext() const { return context; }
        const std::string& getMessage() const { return message; }
        std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
        const std::vector<std::string>& getStackTrace() const { return stackTrace; }

        void setCustomData(const std::string& key, const std::string& value);
        std::string getCustomData(const std::string& key) const;
        const std::map<std::string, std::string>& getAllCustomData() const { return customData; }

        void setContext(const ErrorContext& ctx) { context = ctx; }
        void addMetadata(const std::string& key, const std::string& value);

        std::string getFormattedMessage() const;
        std::string getErrorCodeString() const;
        std::string getSeverityString() const;
        bool isCritical() const { return severity == Severity::CRITICAL; }

        static void addGlobalHandler(ErrorHandler handler);
        static void removeGlobalHandler(std::size_t index);
        static void setStackTraceEnabled(bool enabled) { enableStackTrace = enabled; }
        static void clearGlobalHandlers() { globalHandlers.clear(); }

        static std::string getErrorCodeName(ErrorCode code);
        static std::string getSeverityName(Severity sev);
        static void logException(const GUIException& ex);

    private:
        void captureStackTrace();
        void notifyGlobalHandlers() const;
};

/**
 * @brief Exception for window-related errors
 */
class WindowException : public GUIException {
    public:
        enum class WindowErrorType {
            CREATION_FAILED,
            ALREADY_OPEN,
            ALREADY_CLOSED,
            INVALID_SIZE,
            INVALID_TITLE,
            RENDER_FAILED,
            EVENT_HANDLING_FAILED
        };

    private:
        WindowErrorType windowErrorType;

    public:
        WindowException(const std::string& msg,
                       WindowErrorType type = WindowErrorType::CREATION_FAILED,
                       const ErrorContext& ctx = ErrorContext());

        WindowErrorType getWindowErrorType() const { return windowErrorType; }
        std::string getWindowErrorTypeString() const;
};

/**
 * @brief Exception for widget-related errors
 */
class WidgetException : public GUIException {
    public:
        enum class WidgetErrorType {
            INVALID_POSITION,
            INVALID_SIZE,
            ID_CONFLICT,
            PARENT_NOT_FOUND,
            CHILD_ADDITION_FAILED,
            RENDER_FAILED,
            EVENT_HANDLING_FAILED,
            INVALID_STATE
        };

    private:
        WidgetErrorType widgetErrorType;
        std::string widgetId;

    public:
        WidgetException(const std::string& msg,
                       WidgetErrorType type = WidgetErrorType::INVALID_POSITION,
                       const std::string& id = "",
                       const ErrorContext& ctx = ErrorContext());

        WidgetErrorType getWidgetErrorType() const { return widgetErrorType; }
        const std::string& getWidgetId() const { return widgetId; }
        std::string getWidgetErrorTypeString() const;
};

/**
 * @brief Exception for font and text-related errors
 */
class FontException : public GUIException {
    public:
        enum class FontErrorType {
            LOAD_FAILED,
            NOT_FOUND,
            INVALID_SIZE,
            RENDER_FAILED,
            CHARACTER_NOT_FOUND,
            MEMORY_ERROR
        };

    private:
        FontErrorType fontErrorType;
        std::string fontPath;

    public:
        FontException(const std::string& msg,
                     FontErrorType type = FontErrorType::LOAD_FAILED,
                     const std::string& path = "",
                     const ErrorContext& ctx = ErrorContext());

        FontErrorType getFontErrorType() const { return fontErrorType; }
        const std::string& getFontPath() const { return fontPath; }
        std::string getFontErrorTypeString() const;
};

/**
 * @brief Exception for resource management errors
 */
class ResourceException : public GUIException {
    public:
        enum class ResourceErrorType {
            LOAD_FAILED,
            NOT_FOUND,
            INVALID_FORMAT,
            MEMORY_ERROR,
            PERMISSION_DENIED,
            CORRUPTED_DATA
        };

    private:
        ResourceErrorType resourceErrorType;
        std::string resourcePath;
        std::string resourceType;

    public:
        ResourceException(const std::string& msg,
                         ResourceErrorType type = ResourceErrorType::LOAD_FAILED,
                         const std::string& path = "",
                         const std::string& resourceType = "",
                         const ErrorContext& ctx = ErrorContext());

        ResourceErrorType getResourceErrorType() const { return resourceErrorType; }
        const std::string& getResourcePath() const { return resourcePath; }
        const std::string& getResourceType() const { return resourceType; }
        std::string getResourceErrorTypeString() const;
};

/**
 * @brief Exception for input handling errors
 */
class InputException : public GUIException {
    public:
        enum class InputErrorType {
            INVALID_INPUT,
            VALIDATION_FAILED,
            PARSING_ERROR,
            CONVERSION_FAILED,
            LENGTH_EXCEEDED,
            FORMAT_ERROR
        };

    private:
        InputErrorType inputErrorType;
        std::string inputValue;

    public:
        InputException(const std::string& msg,
                      InputErrorType type = InputErrorType::INVALID_INPUT,
                      const std::string& value = "",
                      const ErrorContext& ctx = ErrorContext());

        InputErrorType getInputErrorType() const { return inputErrorType; }
        const std::string& getInputValue() const { return inputValue; }
        std::string getInputErrorTypeString() const;
};

/**
 * @brief Exception for rendering errors
 */
class RenderException : public GUIException {
    public:
        enum class RenderErrorType {
            DRAW_FAILED,
            TEXTURE_ERROR,
            SHADER_ERROR,
            BUFFER_ERROR,
            CONTEXT_ERROR,
            PERFORMANCE_ERROR
        };

    private:
        RenderErrorType renderErrorType;
        sf::Vector2f renderPosition;

    public:
        RenderException(const std::string& msg,
                       RenderErrorType type = RenderErrorType::DRAW_FAILED,
                       const sf::Vector2f& pos = {0, 0},
                       const ErrorContext& ctx = ErrorContext());

        RenderErrorType getRenderErrorType() const { return renderErrorType; }
        sf::Vector2f getRenderPosition() const { return renderPosition; }
        std::string getRenderErrorTypeString() const;
};

/**
 * @brief Exception for configuration errors
 */
class ConfigurationException : public GUIException {
    public:
        enum class ConfigErrorType {
            INVALID_CONFIG,
            MISSING_KEY,
            INVALID_VALUE,
            PARSE_ERROR,
            FILE_NOT_FOUND,
            PERMISSION_DENIED
        };

    private:
        ConfigErrorType configErrorType;
        std::string configKey;
        std::string configValue;

    public:
        ConfigurationException(const std::string& msg,
                              ConfigErrorType type = ConfigErrorType::INVALID_CONFIG,
                              const std::string& key = "",
                              const std::string& value = "",
                              const ErrorContext& ctx = ErrorContext());

        ConfigErrorType getConfigErrorType() const { return configErrorType; }
        const std::string& getConfigKey() const { return configKey; }
        const std::string& getConfigValue() const { return configValue; }
        std::string getConfigErrorTypeString() const;
};

#define GUI_THROW(msg, code, severity) \
    throw GUI::GUIException(msg, code, severity, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_WINDOW(msg, type) \
    throw GUI::WindowException(msg, type, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_WIDGET(msg, type, id) \
    throw GUI::WidgetException(msg, type, id, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_FONT(msg, type, path) \
    throw GUI::FontException(msg, type, path, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_RESOURCE(msg, type, path, resType) \
    throw GUI::ResourceException(msg, type, path, resType, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_INPUT(msg, type, value) \
    throw GUI::InputException(msg, type, value, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_RENDER(msg, type, pos) \
    throw GUI::RenderException(msg, type, pos, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

#define GUI_THROW_CONFIG(msg, type, key, value) \
    throw GUI::ConfigurationException(msg, type, key, value, GUI::GUIException::ErrorContext(__FUNCTION__, __FILE__, __LINE__));

} // namespace GUI
