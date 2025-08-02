#pragma once

#include <QString>
#include <QDebug>
#include <source_location>

/**
 * @brief Static utility class for debug and error logging with automatic source location tracking
 * 
 * This utility class provides static methods for logging that can be used by any class
 * without requiring inheritance. Uses C++20 std::source_location for automatic 
 * file/line/function tracking.
 * 
 * Usage:
 *   Log::debug("This is a debug message");
 *   Log::error("This is an error message");
 *   Log::warning("This is a warning message");
 *   Log::info("This is an info message");
 * 
 * Configuration:
 *   Log::setLogLevel(Log::LogLevel::Warning);
 *   Log::setIncludeSourceLocation(false);
 */
class Log
{
public:
    // Log level enumeration
    enum class LogLevel {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Off = 4
    };

    // Static logging methods with automatic source location
    static void debug(const QString& message, 
                     const std::source_location& location = std::source_location::current()) {
        logWithLevel(LogLevel::Debug, "DEBUG", message, location);
    }
    
    static void info(const QString& message, 
                    const std::source_location& location = std::source_location::current()) {
        logWithLevel(LogLevel::Info, "INFO", message, location);
    }
    
    static void warning(const QString& message, 
                       const std::source_location& location = std::source_location::current()) {
        logWithLevel(LogLevel::Warning, "WARNING", message, location);
    }
    
    static void error(const QString& message, 
                     const std::source_location& location = std::source_location::current()) {
        logWithLevel(LogLevel::Error, "ERROR", message, location);
    }
    
    // Configuration methods
    static void setLogLevel(LogLevel level) { currentLogLevel_ = level; }
    static LogLevel logLevel() { return currentLogLevel_; }
    
    static void setIncludeSourceLocation(bool include) { includeSourceLocation_ = include; }
    static bool includeSourceLocation() { return includeSourceLocation_; }
    
    // Enable/disable logging entirely
    static void setEnabled(bool enabled) { enabled_ = enabled; }
    static bool isEnabled() { return enabled_; }

private:
    // Private constructor - this is a utility class
    Log() = delete;
    
    // Internal logging implementation
    static void logWithLevel(LogLevel messageLevel,
                           const QString& levelName,
                           const QString& message,
                           const std::source_location& location) {
        
        // Check if logging is enabled and level is appropriate
        if (!enabled_ || messageLevel < currentLogLevel_) {
            return;
        }
        
        QString logMessage;
        
        if (includeSourceLocation_) {
            logMessage = QString("[%1:%2 in %3] %4")
                           .arg(QString::fromLatin1(location.file_name()).split('/').last())
                           .arg(location.line())
                           .arg(QString::fromLatin1(location.function_name()))
                           .arg(message);
        } else {
            logMessage = message;
        }
        
        // Route to appropriate Qt logging function
        switch (messageLevel) {
        case LogLevel::Debug:
        case LogLevel::Info:
            qDebug() << levelName << ":" << logMessage;
            break;
        case LogLevel::Warning:
            qWarning() << levelName << ":" << logMessage;
            break;
        case LogLevel::Error:
            qWarning() << levelName << ":" << logMessage;
            break;
        case LogLevel::Off:
            break;
        }
    }
    
    // Static configuration
    static inline LogLevel currentLogLevel_ = LogLevel::Debug;
    static inline bool includeSourceLocation_ = true;
    static inline bool enabled_ = true;
}; 
