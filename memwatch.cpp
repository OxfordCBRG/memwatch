#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <vector>

using namespace std;

const int SAMPLE_RATE_SECONDS = 2;
const int MAX_SYSTEM_UID = 999; // Ignore all processes owned by UIDs <= MAX_SYSTEM_UID
const int MAX_MEM_PERCENT = 90;
const unsigned long long PAGE_SIZE_BYTES = sysconf(_SC_PAGESIZE);
const unsigned long long MEM_SIZE_BYTES = sysconf(_SC_PHYS_PAGES) * PAGE_SIZE_BYTES;
const unsigned long long MAX_MEM_BYTES = (MEM_SIZE_BYTES * MAX_MEM_PERCENT) / 100;

struct UserStats
{
    unsigned long long total_mem;
    unsigned long long largest_rss;
    string largest_pid;
};

// Read the whole of a file into a single string, removing the final '\n'
inline const string file_to_string(const string &path)
{
    ifstream ifs(path);
    const string contents((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
    return(contents.substr(0,contents.length()-1));
}
 
// Get a file as a vector of strings, splits on any character in [:space:]
inline const vector<string> split_on_space(const string &path)
{
    istringstream iss(path);
    return vector<string>((istream_iterator<string>(iss)),istream_iterator<string>());
}

int main()
{
    
    cout << "INFO: PAGE_SIZE_BYTES\t" << PAGE_SIZE_BYTES << '\n';
    cout << "INFO: MEM_SIZE_BYTES\t" << MEM_SIZE_BYTES << '\n';
    cout << "INFO: MAX_MEM_PERCENT\t" << MAX_MEM_PERCENT << '\n';
    cout << "INFO: MAX_MEM_BYTES\t" << MAX_MEM_BYTES << '\n';
    
    while (true)
    {
        
        unordered_map<uid_t, UserStats> users;
        unsigned long long total_rss = 0;

        for (const auto & pd : filesystem::directory_iterator("/proc/"))
        {
            string pid = "";
            uid_t user = 0;
            unsigned long long mem = 0;
            try
            {
                const auto full_path = pd.path();
                pid = full_path.filename();
                if (! all_of(pid.begin(), pid.end(), ::isdigit)) // Not a PID
                    continue;
                const string pid_root = "/proc/" + pid;

                struct stat statbuf;
                if (lstat(pid_root.c_str(), &statbuf) == -1)
                    continue; // Process probably exited. Don't care.
                user = statbuf.st_uid;

                const auto stats = split_on_space(file_to_string(pid_root + "/statm"));
                mem = (stoull(stats.at(1)) * PAGE_SIZE_BYTES);
            }
            catch (const exception &e)
            {
                cerr << "WARN: " << e.what() << '\n'; // Don't crash, just log it.
                continue;
            }
            
            total_rss += mem;
            if (user <= MAX_SYSTEM_UID)
                continue; // Only care about the total

            users.try_emplace(user, UserStats {0, 0, ""}); // Does nothing if exists
            users[user].total_mem += mem;
            if (mem > users[user].largest_rss)
            {
                users[user].largest_rss = mem;
                users[user].largest_pid = pid;
            }
        }

        if ((total_rss > MAX_MEM_BYTES) && !users.empty()) // If users is empty, only the OOM can save us
        {
            cout << "WARN: total_rss = " << total_rss << '\n';
            UserStats biggest_offender = {0, 0, ""};
            for (const auto & [uid, user] : users)
                if (user.total_mem > biggest_offender.total_mem)
                    biggest_offender = user;

            cout << "WARN: Calling hook on worst offender's largest PID " << biggest_offender.largest_pid << '\n';
            string cmd = "/usr/libexec/memwatch/hook " + biggest_offender.largest_pid;
            system(cmd.c_str());
        }

        sleep(SAMPLE_RATE_SECONDS);
    };
    return 0;
}
