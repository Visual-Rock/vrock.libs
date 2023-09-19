module;

#include <future>
#include <string>
#include <vector>

export module vrock.ui.FileDialog;

export namespace vrock::ui
{
    /**
     * @struct Filter
     * @brief Represents a file filter for file dialog operations.
     */
    struct Filter
    {
        /// The description of the file type.
        std::string description;

        /// The pattern for the file type filter.
        std::string filter;
    };

    /**
     * @file FileDialog.cpp
     * @brief Display an "Open File" dialog synchronously and returns the selected file.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return The selected file path as string.
     */
    std::string open_file( const std::string &title, const std::string &default_path,
                           std::vector<Filter> filter_patterns );

    /**
     * @file FileDialog.cpp
     * @brief Display an "Open Multiple Files" dialog synchronously and returns the selected files.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return A vector of selected file paths as strings.
     */
    std::vector<std::string> open_multiple_files( const std::string &title, const std::string &default_path,
                                                  std::vector<Filter> filter_patterns );

    /**
     * @file FileDialog.cpp
     * @brief Display a "Select Folder" dialog synchronously and returns the selected folder.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @return The selected folder path as string.
     */
    std::string select_folder( const std::string &title, const std::string &default_path );

    /**
     * @file FileDialog.cpp
     * @brief Display a "Save File" dialog synchronously and returns the selected file.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return The selected file path as string.
     */
    std::string save_file( const std::string &title, const std::string &default_path,
                           std::vector<Filter> filter_patterns );

    /**
     * @file FileDialog.cpp
     * @brief Display an "Open File" dialog asynchronously and returns a future to the selected file path.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return A future to the selected file path as string.
     */
    std::future<std::string> open_file_async( const std::string &title, const std::string &default_path,
                                              const std::vector<Filter> &filter_patterns );

    /**
     * @file FileDialog.cpp
     * @brief Display an "Open Multiple Files" dialog asynchronously and returns a future to the selected files paths.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return A future to the vector of selected file paths as strings.
     */
    std::future<std::vector<std::string>> open_multiple_files_async( const std::string &title,
                                                                     const std::string &default_path,
                                                                     const std::vector<Filter> &filter_patterns );

    /**
     * @file FileDialog.cpp
     * @brief Display a "Select Folder" dialog asynchronously and returns a future to the selected folder.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @return A future to the selected folder path as string.
     */
    std::future<std::string> select_folder_async( const std::string &title, const std::string &default_path );

    /**
     * @file FileDialog.cpp
     * @brief Display a "Save File" dialog asynchronously and returns a future to the selected file.
     * @param title The title of the dialog.
     * @param default_path The path where the dialog will initially point to.
     * @param filter_patterns Vector of Filter struct defining the file types filter.
     * @return A future to the selected file path as string.
     */
    std::future<std::string> save_file_async( const std::string &title, const std::string &default_path,
                                              const std::vector<Filter> &filter_patterns );
} // namespace vrock::ui