/*
    ProcessManager implementation for UCI chess engine communication.
    Based on Microsoft's anonymous pipe example for child process I/O redirection.
    
    Author: Nicolas Miller
    Date: 08/01/2025
*/

#include "process_manager.h"
#include <windows.h>
#include <iostream>
#include <sstream>

ProcessManager::ProcessManager()
    : child_stdin_read_(NULL),
      child_stdin_write_(NULL), 
      child_stdout_read_(NULL),
      child_stdout_write_(NULL),
      process_handle_(NULL),
      thread_handle_(NULL),
      process_id_(0)
{
}

ProcessManager::~ProcessManager()
{
    stop_process();
    cleanup_handles();
}

bool ProcessManager::start_process(const std::string& executable_path)
{
    if (is_running()) 
    {
        std::cerr << "Process is already running" << std::endl;
        return false;
    }

    // Create pipes for child process communication
    if (!create_pipes()) 
    {
        std::cerr << "Failed to create pipes" << std::endl;
        return false;
    }

    // Launch the child process with redirected I/O
    if (!launch_child_process(executable_path)) 
    {
        std::cerr << "Failed to launch child process: " << executable_path << std::endl;
        cleanup_handles();
        return false;
    }

    return true;
}

void ProcessManager::stop_process()
{
    if (!is_running()) return;
    

    // Close stdin pipe to signal child to exit gracefully
    if (child_stdin_write_ != NULL) 
    {
        CloseHandle(child_stdin_write_);
        child_stdin_write_ = NULL;
    }

    // Wait for process to terminate
    DWORD wait_result = WaitForSingleObject(process_handle_, PROCESS_WAIT_TIMEOUT_MS);
    
    if (wait_result == WAIT_TIMEOUT) 
    {
        std::cerr << "Process did not exit gracefully, terminating forcefully" << std::endl;
        TerminateProcess(process_handle_, 1);
        WaitForSingleObject(process_handle_, PROCESS_FORCE_WAIT_MS);
    }

    cleanup_handles();
}

bool ProcessManager::is_running() const
{
    if (process_handle_ == NULL) return false;
    
    DWORD exit_code;
    if (!GetExitCodeProcess(process_handle_, &exit_code)) return false;

    return exit_code == STILL_ACTIVE;
}

bool ProcessManager::write_to_child(const std::string& data)
{
    if (!is_running() || child_stdin_write_ == NULL) return false;
   

    DWORD bytes_written;
    BOOL success = WriteFile(
        child_stdin_write_,
        data.c_str(),
        static_cast<DWORD>(data.length()),
        &bytes_written,
        NULL
    );

    if (!success) 
    {
        DWORD error = GetLastError();
        std::cerr << "WriteFile failed with error: " << error << std::endl;
        return false;
    }

    // Ensure all data was written
    if (bytes_written != data.length()) 
    {
        std::cerr << "Partial write: expected " << data.length() 
                  << " bytes, wrote " << bytes_written << " bytes" << std::endl;
        return false;
    }

    // Flush the pipe to ensure data is sent immediately
    FlushFileBuffers(child_stdin_write_);
    return true;
}


bool ProcessManager::read_line_from_child(std::string& line)
{
    int attempts = 0;
    
    while (attempts++ < MAX_READ_ATTEMPTS)
    {
        // Try to extract complete line from buffer first
        size_t newline_pos = read_buffer_.find('\n');
        if (newline_pos != std::string::npos)
        {
            line = read_buffer_.substr(0, newline_pos);
            // Handle both \r\n and \n line endings
            if (!line.empty() && line.back() == '\r') 
            {
                line.pop_back();
            }
            read_buffer_.erase(0, newline_pos + 1);
            return true;
        }

        // No complete line in buffer; try to read more data
        std::string new_data;
        if (!read_from_child_non_blocking(new_data))
        {
            // No more data available right now
            // If we have partial data in buffer and many attempts, it might be a line without newline
            if (read_buffer_.length() > 0 && attempts > PARTIAL_LINE_THRESHOLD) 
            {
                // Return partial buffer as a line
                line = read_buffer_;
                read_buffer_.clear();
                return true;
            }
            return false;
        }
        
        read_buffer_ += new_data;
    }
    
    // If we've made too many attempts and have data, return it
    if (!read_buffer_.empty()) 
    {
        line = read_buffer_;
        read_buffer_.clear();
        return true;
    }
    
    return false;
}

bool ProcessManager::create_pipes()
{
    SECURITY_ATTRIBUTES security_attr;
    security_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attr.bInheritHandle = TRUE;
    security_attr.lpSecurityDescriptor = NULL;

    // Create pipe for child's STDOUT (child writes, parent reads)
    if (!CreatePipe(&child_stdout_read_, &child_stdout_write_, &security_attr, PIPE_BUFFER_SIZE)) 
    {
        std::cerr << "Failed to create stdout pipe" << std::endl;
        return false;
    }

    // Ensure the read handle is not inherited by child
    if (!SetHandleInformation(child_stdout_read_, HANDLE_FLAG_INHERIT, 0)) 
    {
        std::cerr << "Failed to set stdout read handle information" << std::endl;
        CloseHandle(child_stdout_read_);
        CloseHandle(child_stdout_write_);
        return false;
    }

    // Create pipe for child's STDIN (parent writes, child reads)
    if (!CreatePipe(&child_stdin_read_, &child_stdin_write_, &security_attr, PIPE_BUFFER_SIZE)) 
    {
        std::cerr << "Failed to create stdin pipe" << std::endl;
        CloseHandle(child_stdout_read_);
        CloseHandle(child_stdout_write_);
        return false;
    }

    // Ensure the write handle is not inherited by child
    if (!SetHandleInformation(child_stdin_write_, HANDLE_FLAG_INHERIT, 0)) 
    {
        std::cerr << "Failed to set stdin write handle information" << std::endl;
        cleanup_handles();
        return false;
    }

    return true;
}

bool ProcessManager::launch_child_process(const std::string& executable_path)
{
    PROCESS_INFORMATION process_info;
    STARTUPINFOA startup_info;

    // Initialize structures
    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&startup_info, sizeof(STARTUPINFO));

    // Configure startup info for I/O redirection
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.hStdError = child_stdout_write_;   // Redirect stderr to stdout pipe
    startup_info.hStdOutput = child_stdout_write_;  // Child writes output here
    startup_info.hStdInput = child_stdin_read_;     // Child reads input from here
    startup_info.dwFlags |= STARTF_USESTDHANDLES;

    // Create a mutable copy of the executable path
    std::string cmd_line = executable_path;

    // Create the child process
    BOOL success = CreateProcessA(
        NULL,                           // No module name (use command line)
        &cmd_line[0],                   // Command line (mutable)
        NULL,                           // Process handle not inheritable
        NULL,                           // Thread handle not inheritable
        TRUE,                           // Set handle inheritance to TRUE
        CREATE_NO_WINDOW,               // Hide console window
        NULL,                           // Use parent's environment block
        NULL,                           // Use parent's starting directory
        &startup_info,                  // Pointer to STARTUPINFO structure
        &process_info                   // Pointer to PROCESS_INFORMATION structure
    );

    if (!success) 
    {
        DWORD error = GetLastError();
        std::cerr << "CreateProcess failed with error: " << error << std::endl;
        return false;
    }

    // Store process information
    process_handle_ = process_info.hProcess;
    thread_handle_ = process_info.hThread;
    process_id_ = process_info.dwProcessId;

    // Close handles that are no longer needed by parent
    // Child process inherited copies of these handles
    CloseHandle(child_stdout_write_);
    child_stdout_write_ = NULL;
    
    CloseHandle(child_stdin_read_);
    child_stdin_read_ = NULL;

    std::cout << "Successfully launched process: " << executable_path 
              << " (PID: " << process_id_ << ")" << std::endl;

    return true;
}

void ProcessManager::cleanup_handles()
{
    // Close pipe handles
    if (child_stdin_read_ != NULL) 
    {
        CloseHandle(child_stdin_read_);
        child_stdin_read_ = NULL;
    }
    
    if (child_stdin_write_ != NULL) 
    {
        CloseHandle(child_stdin_write_);
        child_stdin_write_ = NULL;
    }
    
    if (child_stdout_read_ != NULL) 
    {
        CloseHandle(child_stdout_read_);
        child_stdout_read_ = NULL;
    }
    
    if (child_stdout_write_ != NULL) 
    {
        CloseHandle(child_stdout_write_);
        child_stdout_write_ = NULL;
    }

    // Close process handles
    if (thread_handle_ != NULL) 
    {
        CloseHandle(thread_handle_);
        thread_handle_ = NULL;
    }
    
    if (process_handle_ != NULL) 
    {
        CloseHandle(process_handle_);
        process_handle_ = NULL;
    }

    process_id_ = 0;
}

bool ProcessManager::read_from_child_non_blocking(std::string& data)
{
    if (!is_running() || child_stdout_read_ == NULL) 
    {
        return false;
    }
    
    char buffer[READ_BUFFER_SIZE];
    DWORD bytes_read = 0;
    DWORD total_bytes_available = 0;
    
    // Check if data is available
    if (!PeekNamedPipe(child_stdout_read_, NULL, 0, NULL, &total_bytes_available, NULL))
    {
        DWORD error = GetLastError();
        if (error == ERROR_BROKEN_PIPE)
        {
            // Pipe broken, child process likely terminated
            return false;
        }
        // Other errors might resolve on next attempt
        return false;
    }
    
    if (total_bytes_available == 0)
    {
        return false; // No data available
    }
    
    // Read available data (up to buffer size)
    DWORD bytes_to_read = std::min(total_bytes_available, READ_BUFFER_SIZE - 1);
    
    BOOL success = ReadFile(
        child_stdout_read_,
        buffer,
        bytes_to_read,
        &bytes_read,
        NULL
    );
    
    if (!success || bytes_read == 0)
    {
        DWORD error = GetLastError();
        if (error != ERROR_BROKEN_PIPE)
        {
            // Only log non-pipe-broken errors
            std::cerr << "ReadFile failed with error: " << error << std::endl;
        }
        return false;
    }
    
    buffer[bytes_read] = '\0';
    data.assign(buffer, bytes_read);
    return true;
}
