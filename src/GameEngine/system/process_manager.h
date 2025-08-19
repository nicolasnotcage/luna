/*
    ProcessManager header for UCI chess engine communication.
    Manages Windows process creation and anonymous pipe communication.
    
    Author: Nicolas Miller
    Date: 08/01/2025
*/

#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <string>
// Need this preprocessor definition to prevent naming conflicts 
// with Windows defined min-max functions (this was a debugging pain); for some reason,
// we can also throw functions with a naming conflict in parenthesis and this
// solves the error (e.g., (std::min()))?
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class ProcessManager 
{
public:
    ProcessManager();
    ~ProcessManager();
    
    // Non-copyable
    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;
    
    // Core process management
    bool start_process(const std::string& executable_path);
    void stop_process();
    bool is_running() const;
    
    // Communication interface for UCIClient
    bool write_to_child(const std::string& data);
    bool read_line_from_child(std::string& line);

private:
    // Constants
    static constexpr DWORD PIPE_BUFFER_SIZE = 65536;
    static constexpr DWORD READ_BUFFER_SIZE = 4096;
    static constexpr DWORD PROCESS_WAIT_TIMEOUT_MS = 5000;
    static constexpr DWORD PROCESS_FORCE_WAIT_MS = 2000;
    static constexpr int MAX_READ_ATTEMPTS = 1000;
    static constexpr int PARTIAL_LINE_THRESHOLD = 50;
    
    // Pipe handles for child communication
    HANDLE child_stdin_read_;    // Child reads from this
    HANDLE child_stdin_write_;   // Parent writes to this  
    HANDLE child_stdout_read_;   // Parent reads from this
    HANDLE child_stdout_write_;  // Child writes to this
    
    // Process management
    HANDLE process_handle_;
    HANDLE thread_handle_;
    DWORD process_id_;

    // Buffer management
    std::string read_buffer_;
    
    // Internal implementation
    bool create_pipes();
    bool launch_child_process(const std::string& executable_path);
    void cleanup_handles();
    bool read_from_child_non_blocking(std::string& data);
};

#endif // PROCESS_MANAGER_H
